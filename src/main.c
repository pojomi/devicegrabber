#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/dg.h"

int vendor = 0;
int product = 0;
int daemon_call = 0;

int main(int argc, char **argv) {
  if (argc == 1) {
    dg_get_event_files();
    return DG_RET_SUCCESS;
  }
  if (strcmp(argv[1], "--daemon") == 0) {
    daemon_call = 1;
    vendor = atoi(argv[2]);
    product = atoi(argv[3]);
    printf("\e[32mVendor arg2: %d Product arg3: %d\e[m\n", vendor, product);
    dg_get_event_files();
    return DG_RET_SUCCESS;
  }
  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    dg_print_help();
    return DG_RET_SUCCESS;
  }
}
