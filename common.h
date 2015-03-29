/*
 * Various data types and functions shared across the program
 */

#ifndef COMMON_H
#define COMMON_H

#undef SECTOR_SIZE 
#define SECTOR_SIZE	(5)

#define CMD_LENGTH	(5)
#define FOREVER		(1)
#define COMMAND_WRITE	("::w")
#define COMMAND_READ	("::r")
#define COMMAND_DAMAGE	("::d")
#define COMMAND_EXIT	("::e")


/* Defined when compiling */
#ifdef DEBUG

#define DEBMSG(msg)	(printf("## %s:%d: " #msg " is \"%s\" (in %s)\n",			\
		       	__FILE__, __LINE__, (msg), __FUNCTION__))

#define DEBINFO(exp)	(printf("## %s:%d: " #exp " evaluates to %d (in %s)\n",	\
		       	__FILE__, __LINE__, (exp), __FUNCTION__))

#else

#define DEBMSG(msg)	do {} while (0)
#define DEBINFO(exp)	do {} while (0)

#endif

#include <stdio.h>
#include <stdlib.h>

/* Result types for function returns */
enum hdd_result {
	HDD_SUCCESS			= 1, 	
	HDD_ERROR_INVALID_PARAMETER	= 2,	/* parameter is invalid */
	HDD_ERROR_MEMORY_ALLOC		= 3,	/* error allocating memory */
	HDD_ERROR_INVALID_RESOURCE	= 4,	/* resource not present */
	HDD_ERROR_INVALID_ARGUMENTS	= 5,	
	HDD_ERROR_FILE_ACCESS		= 6,	/* cannot open file */
	HDD_SEEK_INCOMPLETE		= 7,	
	HDD_ERROR_END_OF_FILE		= 8,	/* unexpected end of file */
};

/* The address on the hard drive */
struct hdd_address {
	int line;
	int index;
};

void hdd_print_result(enum hdd_result msg);

#endif	/* #ifndef COMMON_H */
