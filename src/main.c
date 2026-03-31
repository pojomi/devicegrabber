#include <string.h>
#include "../include/dg.h"

int main(int argc, char **argv) {
	if (argc == 1) {
		dg_get_event_files();
		return DG_RET_SUCCESS;
	}
	else if (strcmp(argv[1], "-F") == 0 || strcmp(argv[1], "-f") == 0
									 || strcmp(argv[1], "--file-name") == 0) {
		dg_run(argv[2], NULL);
		return DG_RET_SUCCESS;
	}
}
