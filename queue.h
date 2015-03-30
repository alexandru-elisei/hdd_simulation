/*
 * The queue for executing commands. The first command added to the queue is
 * the command that gets executed. When the time limit expires, another command
 * is added to the queue. If all the commands in the queue are executed before
 * the time expires then the cursor idles on the current sector.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#include "common.h"
#include "hdd.h"

struct command_queue {
	struct hdd_address *addr;	/* Address of the command */
	struct command_queue *next;	/* Next command in queue */
	char cmd[CMD_LENGTH];		/* Literal text of the command */
	uint8_t data_count;	
	char data[SECTOR_SIZE];		/* Data to be written/read */
};

void cq_init(struct command_queue **t, struct command_queue **h);

/* Adds a command to the queue */
enum hdd_result cq_enqueue(struct command_queue **t,
			   struct command_queue **h, 
			   char *buf); 

/* Executes a command */
enum hdd_result cq_execute(struct command_queue *head,
			struct hdd_head *h,
			FILE *out);

/* Prints the entire command queue */
void cq_print(struct command_queue *t); 

/* Checks if no commands are prending */
int cq_is_empty(const struct command_queue *h);

#endif	/* #ifndef QUEUE_H */
