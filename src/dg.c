#include <limits.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include "../include/dg.h"

char *device_name = NULL;
char **event_files, **device_names;
int **device_product_ids, **device_vendor_ids;
size_t count = 0;

void dg_print_help() {
  printf("\nUSAGE: devicegrabber [OPTION]... [FILE]\n"
	 "       devicegrabber\n\n"
	 "Arguments required for long and short options\n"
	 "  \e[1m-F, -f, --filename\e[m\n"
	 "      /dev/input/eventX\n\n"
	 "  \e[1m-h, --help\e[m\n\n");
}

void dg_free(void **arr, size_t count) {
  for (int i = 0; i < count; i++) {
    if (arr[i] == NULL) continue;
    free((void*)arr[i]);
  }
  free(arr);
}

// Realloc a double pointer by (initial * mult) and return as a new array
void **dg_realloc_arr(void **arr, size_t count) {
  void **new_arr = (void**)realloc(arr, (count + REALLOC_DEF) * sizeof(void *));
  if (new_arr == NULL) return NULL;
  return new_arr;
}

void dg_free_all_and_exit(void **arr1, void **arr2, size_t count) {
  dg_free(arr1, count);
  dg_free(arr2, count);
  exit(DG_EXIT_SUCCESS);
}

void dg_get_event_files() {
  DIR *dirp;
  struct dirent *entry;
  struct libevdev *dev = NULL;
  char *devpath = "/dev/input/"; // For concatenation
  event_files = (char **)malloc(MIN_ALLOC * sizeof(char *)); // List of event files
  char event_path[256]; // </dev/input/eventX> buffer
  int response;
  size_t size = MIN_ALLOC;

  dirp = opendir(devpath);
  if (dirp == NULL) {
    free(event_files);
    perror("\e[31mopendir failed\e[m\n");
    exit(DG_EXIT_FAIL);
  }

  while ((entry = readdir(dirp)) != NULL) {

    if (strncmp(entry->d_name, "event", 5) != 0) {
      continue;
    }

    memset(event_path, 0, sizeof(event_path));
    int bytes = sprintf(event_path, "%s%s", devpath, entry->d_name); // Set path
    if (bytes <= 0) {
      free(event_files);
      perror("\e[31mdevpath sprintf failed\e[m\n");
      exit(DG_EXIT_FAIL);
    }

    event_files[count] = (char*)malloc(strlen(entry->d_name) * sizeof(char)); // eventX
    if (event_files[count] == NULL) {
        dg_free((void**)event_files, count);
        closedir(dirp);
        perror("\e[31mmalloc event_files NULL\e[m\n");
        exit(DG_EXIT_FAIL);
    }

    strcpy(event_files[count], (const char *)event_path); // strcpy to grab the null-terminator
    if (event_files[count] == NULL) {
        dg_free((void**)event_files, count);
        closedir(dirp);
        perror("\e[31mNull copy \e[m\n");
        exit(DG_EXIT_FAIL);
    }

    count++;

    if (count == size + 1) {
      char **add_to_event_files = (char **)dg_realloc_arr((void**)event_files, count);
      if (add_to_event_files == NULL) {
        closedir(dirp);
        dg_free((void**)event_files, count);
        perror("\e[31mFailed reallocate array size\e[m\n");
        exit(DG_EXIT_FAIL);
      }

      event_files = add_to_event_files;
      size+= REALLOC_DEF;
    }
  }
  closedir(dirp);
  dg_get_device_info();

  dg_free((void**)event_files, count);
}

void dg_get_device_info() {
  struct libevdev *dev = NULL;
  int fd, rc = 1;
  device_names = (char **)malloc((count * sizeof(char *))); // List of device names
  device_product_ids = (int **)malloc((count * sizeof(int *)));
  device_vendor_ids = (int **)malloc((count * sizeof(int *)));

  char *name = NULL; // To hold name before memcpy into name_list
  char *daemon_fp = NULL;
  // Use libevdev calls to get device names and store in name_list
  for (int i = 0; i < count; i++) {
    fd = open(event_files[i], O_RDONLY | O_NONBLOCK); // O_RDONLY here since
    if (fd == -1) {                                  // we're not grabbing
      close(fd);
      dg_free((void**)device_names, count);
      dg_free((void**)device_product_ids, count);
      dg_free((void**)device_vendor_ids, count);
      return;
    }
    rc = libevdev_new_from_fd(fd, &dev);              
    if (rc < 0) {
      libevdev_free(dev);
      close(fd);
      dg_free((void**)device_names, count);
      dg_free((void**)device_product_ids, count);
      dg_free((void**)device_vendor_ids, count);
      return;
    }

    name = (char *)libevdev_get_name(dev); // Store name to be copied
    device_names[i] = malloc(strlen(name) * sizeof(char));
    // Product ID
    device_product_ids[i] = malloc(sizeof(int));
    *device_product_ids[i] = libevdev_get_id_product(dev);
    // Vendor ID
    device_vendor_ids[i] = malloc(sizeof(int));
    *device_vendor_ids[i] = libevdev_get_id_vendor(dev);

    if (device_names[i] == NULL) {
      libevdev_free(dev);
      close(fd);
      perror("\e[31mNull device_names index. Exiting\e[m\n");
      dg_free((void**)device_names, count);
      dg_free((void**)device_product_ids, count);
      dg_free((void**)device_vendor_ids, count);
      return;
    }

    strcpy(device_names[i], (const char*)name); // strcpy to grab the null-terminator

    if (daemon_call == 1) {
      if (*device_vendor_ids[i] == vendor && *device_product_ids[i] == product) {
	daemon_fp = event_files[i];
	device_name = device_names[i];
	break;
      }
    }

    close(fd);
  }

  libevdev_free(dev);
  if (vendor > 0 && product > 0) {
    dg_run(daemon_fp);
  } else {
    dg_print_list();
  }
}

void dg_print_list() {
  for (int i = 0; i < count; i++) {
    printf("%d) %s\n", i + 1, device_names[i]);
    usleep(2000);
  }
  dg_get_selection();
}

void dg_get_selection() {
  int response;

  printf("Select device (0 to exit): ");
  scanf("%d", &response);
  if (response == 0) {
      printf("Exiting...\n");
      dg_free_all_and_exit((void**)device_names, (void**)event_files, count);
  }
  if (response > count) {
    printf("\nInvalid choice\n\n");
    sleep(1);
    dg_print_list();
  }
    
  response--;
  printf("Selected %s located at %s\n", device_names[response],
         event_files[response]);
  device_name = device_names[response];
  product = *device_product_ids[response];
  vendor = *device_vendor_ids[response];

  dg_get_run_option(response);
}

void dg_get_run_option(int response) {
  printf("\nSelect Option\n");
  printf("0) Exit\n1) Run once (Immediately)\n2) Create systemd daemon (starts "
         "immediately)\n> ");

  int new_response;
  scanf("%d", &new_response);
  if (new_response == DG_RUN_NOW) {
    dg_run(event_files[response]);
  }
  if (new_response == DG_MAKE_DAEMON) {
    dg_write_service_file();
  }
  if (new_response == DG_SELECT_EXIT) {
    printf("Exiting...\n");
  }
  dg_free((void**)device_product_ids, count);
  dg_free((void**)device_vendor_ids, count);
  dg_free_all_and_exit((void **)device_names, (void **)event_files, count);
}

void dg_run(char *event_file) {
  struct libevdev *dev = NULL;
  int fd, rc = 1;

  fd = open(event_file, O_RDWR | O_NONBLOCK); // O_RDWR here for the device grab
  if (fd < 0) {
    printf("\e[31mFailed to open %s(are you root?)\e[m\n", event_file);
    return;
  } else printf("File opened\n");

  rc = libevdev_new_from_fd(fd, &dev);
  if (rc < 0) {
    perror("\e[31mlibevdev_new_from_fd failed\e[m\n");
    return;
  } else printf("libevdev initialized\n");

  const char *name = libevdev_get_name(dev);
  if (device_name != NULL) { 
    if (strcmp(name, (const char *)device_name) != 0) {
      printf("Names don't match: libevdev: %s --- device_name: %s\n", name, device_name);
      libevdev_free(dev);
      close(fd);
      return;
    }
  }
  
  int grab = libevdev_grab(dev, LIBEVDEV_GRAB);
  if (grab == 0) {
    printf("%s grabbed!\nUse terminal exit key to ungrab (common default is Ctrl-C)\n", name);

    while (1) {
      sleep(INT_MAX);
    }

  } else {
    perror("\e[31mDevice grab failed\e[m\n");
    close(fd);
  }
  libevdev_free(dev);
  close(fd);
}

void dg_write_service_file() {
  FILE *f = NULL;
  int bytes_written = 0;
  char service_name[256];
  memset(service_name, 0, sizeof(service_name));

  // Replace spaces with hyphens for appropriate file name
  sprintf(service_name, "/etc/systemd/system/devicegrabber-%d-%d.service", vendor, product);
  char *service_only = strstr(service_name, "devicegrabber");

  f = fopen((const char*)service_name, "wb");
  if (f == NULL) {
    perror("\e[31mFailed to make file\e[m\n");
    return;
  }

  bytes_written = fprintf(f, "[Unit]\nDescription=devicegrabber daemon\n"
			  "\n[Service]\nType=simple\n"
			  "ExecStart=/usr/bin/devicegrabber --daemon %d %d\n"
			  "Restart=on-failure\n\n"
                          "[Install]\nWantedBy=default.target\n", vendor, product);
  if (bytes_written <= 0) {
    printf("Failed to write\n");
    fclose(f);
    return;
  } else {
    printf("Wrote %d bytes to %s\n", bytes_written, service_name);
    fclose(f);
    dg_handle_systemd(service_only);
  }
}

void dg_handle_systemd(char *service_name) {
  char command[256];
  memset(command, 0, sizeof(command));
  sprintf(command, "systemctl daemon-reload && systemctl enable --now %s && systemctl start --now %s",
	  service_name, service_name);
  system(command); 
}
