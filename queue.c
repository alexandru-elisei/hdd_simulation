#include <string.h>

#include "queue.h"

void cq_init(struct command_queue **h, struct command_queue **t)
{
	*h = *t = NULL;
}
	
/*
*q = (struct hdd_queue *) malloc(sizeof(struct hdd_queue));
	if (*q == NULL)
		return HDD_ERROR_MEMORY_ALLOC;
	
	(*q)->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if ((*q)->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;
	(*q)->next = NULL;

	return HDD_SUCCESS;
}
*/

/* Extracts and adds a command from the input */
enum hdd_result queue(struct command_queue **q, char *input);
