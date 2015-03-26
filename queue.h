/*
 * The queue for executing commands is implemented. The first command added
 * to the queue is executed. When the time limit expires, another command is
 * added to the queue. If all the commands in the queue were executed before the
 * time expired then the cursor simply waits on the current sector.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#include "common.h"

struct command_queue {
	struct hdd_address *addr;	/* Address of the command */
	struct command_queue *next;	/* Next command in queue */
	char cmd[CMD_STR_LENGTH];	/* Literal text of the command */
	uint8_t is_chained;		/* If the command is part of a chain */
};

void cq_init(struct command_queue **t, struct command_queue **h);

/* Adds a command to the queue */
enum hdd_result cq_enqueue(struct command_queue **t,
			   struct command_queue **h, 
			   struct hdd_address *a,
			   char *cmd); 

/* Executes a command */
enum hdd_result cq_execute(struct command_queue **h);

/* Removes a command */
struct command_queue *cq_dequeue(struct command_queue **h); 

/* Shows the current command */
struct command_queue *cq_peek(struct command_queue *h); 

/* Prints the entire command queue in reverse */
void *cq_print(struct command_queue *t); 

#endif	/* #ifndef QUEUE_H */
