#ifndef __DG_H__
#define __DG_H__

#define MIN_ALLOC 10
#define REALLOC_DEF 10
#define DG_RET_SUCCESS 0
#define DG_RET_FAIL -1
#define DG_EXIT_SUCCESS -2
#define DG_EXIT_FAIL -3
#define DG_SELECT_EXIT 0
#define DG_RUN_NOW 1
#define DG_MAKE_DAEMON 2

typedef unsigned long size_t;
extern int vendor, product, daemon_call;

void dg_print_help();
void dg_free(void **arr, size_t count);
void **dg_realloc_arr(void **arr, size_t count);
void dg_free_all_and_exit(void **arr1, void **arr2, size_t count);
void dg_get_event_files();
void dg_get_device_info();
void dg_print_list();
void dg_get_selection();
void dg_get_run_option(int response);
void dg_run(char *event_file);
void dg_write_service_file();
void dg_handle_systemd(char *service_name);
#endif
