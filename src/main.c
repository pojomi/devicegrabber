#include <string.h>
#include "../include/dg.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		dg_get_event_files();
		return DG_RET_SUCCESS;
	}
	if (strcmp(argv[1], "-F") == 0 || strcmp(argv[1], "-f") == 0
									 || strcmp(argv[1], "--filename") == 0) {
		dg_run(argv[2], NULL);
		return DG_RET_SUCCESS;
	}
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		dg_print_help();
		return DG_RET_SUCCESS;
	}
}
