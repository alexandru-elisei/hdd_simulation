/*
 * Various data types and functions shared across the program
 */

#pragma once

/* Defined when compiling */
#if DEBUG

#define DEBMSG(text) 	(printf("%s\n", (text)))
#define DEBINFO(exp)	(printf("%s evaluates to %d\n", #exp, (exp)))

#else

#define DEBMSG(text)	/* empty */
#define DEBINFO(exp)	/* empty */

#endif

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	HDD_SUCCESS			= 1, 	
	HDD_ERROR_INVALID_PARAMETER	= 2,	/* parameter is invalid */
	HDD_ERROR_MEMORY_ALLOC		= 3,	/* error allocating memory */
} hdd_result_t;

void hdd_print_result(hdd_result_t msg);
