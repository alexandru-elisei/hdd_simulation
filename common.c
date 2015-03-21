#include "common.h"

void hdd_print_result(hdd_result_t msg)
{
	switch (msg) {
	case HDD_SUCCESS:
		fprintf(stderr, "HDD: Operation executed successfully.\n");
		break;
	case HDD_ERROR_INVALID_PARAMETER:
		fprintf(stderr, "HDD: Invalid parameter supplied.\n");
		break;
	case HDD_ERROR_MEMORY_ALLOC:
		fprintf(stderr, "HDD: Error allocating memory.\n");
		break;
	}
}
