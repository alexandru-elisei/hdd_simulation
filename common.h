/*
 * Various data types and functions shared across the program
 */

#pragma once

/* Defined when compiling */
#if DEBUG

#define DEBINFO(exp)	(printf("%s evaluates to %d\n", #exp, (exp)))

#else

#define DEBINFO(exp)	do {} while (0)

#endif

#include <stdio.h>
#include <stdlib.h>

/* Result types for function returns */
enum hdd_result {
	HDD_SUCCESS			= 1, 	
	HDD_ERROR_INVALID_PARAMETER	= 2,	/* parameter is invalid */
	HDD_ERROR_MEMORY_ALLOC		= 3,	/* error allocating memory */
};

void hdd_print_result(enum hdd_result msg);
