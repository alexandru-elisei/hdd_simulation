#include <string.h>

#include "queue.h"

void cq_init(struct command_queue **t, struct command_queue **h)
{
	*h = *t = NULL;
}
	
/* Adds a command to the queue */
enum hdd_result cq_enqueue(struct command_queue **t,
			   struct command_queue **h, 
			   struct hdd_address *a,
			   char *cmd)
{
	struct command_queue *new;

	new = (struct command_queue *) malloc(sizeof(struct command_queue));
	if (new == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (new->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;
	new->addr->line = a->line;
	new->addr->index = a->index;
	strncpy(new->cmd, cmd, CMD_STR_LENGTH - 1);
	
	if (*t == NULL) {
		*t = *h = new;
		(*t)->next = *h;
		(*h)->next = NULL;
		return HDD_SUCCESS;
	}

	new->next = *t;
	*t = new;

	return HDD_SUCCESS;
}
