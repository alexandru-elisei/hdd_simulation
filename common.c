#include "common.h"

void hdd_print_result(enum hdd_result msg)
{
	/* I don't like switch */
	if (msg == HDD_SUCCESS)
		fprintf(stderr, "HDD: Operation executed successfully.\n");
	else if (msg == HDD_ERROR_INVALID_PARAMETER)
		fprintf(stderr, "HDD: Invalid parameter supplied.\n");
	else if (msg == HDD_ERROR_MEMORY_ALLOC)
		fprintf(stderr, "HDD: Error allocating memory.\n");
	else if (msg == HDD_ERROR_INVALID_RESOURCE)
		fprintf(stderr, "HDD: Accessing invalid resource.\n");
	else if (msg == HDD_ERROR_INVALID_ARGUMENTS)
		fprintf(stderr, "HDD: Invalid arguments to program call.\n");
	else if (msg == HDD_ERROR_FILE_ACCESS)
		fprintf(stderr, "HDD: Cannot access file.\n");
}
