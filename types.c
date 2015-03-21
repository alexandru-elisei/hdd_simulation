#include "types.h"

void hdd_print_result(hdd_result_t msg)
{
	switch (msg) {
	HDD_SUCCESS:
		fprintf(stderr, "HDD: Operation executed successfully.\n");
		break;
	HDD_ERROR_INVALID_PARAM:
		fprintf(stderr, "HDD: Invalid parameter supplied.\n");
		break;
	HDD_ERROR_MEMORY_ALLOC:
		fprintf(stderr, "HDD: Error allocating memory.\n");
		break;
	}
}
