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
	struct hdd_address *addr;
	struct command_queue *next;
	char cmd[CMD_STR_LENGTH];
	uint8_t is_chained;
};

void cq_init(struct command_queue **h, struct command_queue **t);

/* Extracts and adds a command from the input */
enum hdd_result cq_enqueue(struct command_queue **t, char *cmd); 

/* Removes a command */
struct command_queue *cq_dequeue(struct command_queue **h); 

/* Shows the current command */
struct command_queue *cq_peek(struct command_queue *h); 

#endif	/* #ifndef QUEUE_H */
