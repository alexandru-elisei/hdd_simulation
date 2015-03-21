/*
 * Various custom data types
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	HDD_SUCCESS			= 1, 	
	HDD_ERROR_INVALID_PARAMETER	= 2,	/* parameter is invalid */
	HDD_ERROR_MEMORY_ALLOC		= 3,	/* error allocating memory */
} hdd_result_t;

void hdd_print_result(hdd_result_t msg);
