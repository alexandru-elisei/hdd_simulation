/*
 * Various data types and functions shared across the program
 */

#pragma once

#if DEBUG

#define DEBMSG(msg) (printf("#(msg)\n"))
#define DEBINFO(exp)	(printf("#(exp) evaluates to %d\n", (exp)))

#else

#define DEBMSG(MSG)	()
#define DEBINFO(exp)	()

#endif

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	HDD_SUCCESS			= 1, 	
	HDD_ERROR_INVALID_PARAMETER	= 2,	/* parameter is invalid */
	HDD_ERROR_MEMORY_ALLOC		= 3,	/* error allocating memory */
} hdd_result_t;

void hdd_print_result(hdd_result_t msg);
