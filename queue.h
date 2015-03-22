/*
 * The queue for executing commands is implemented. The first command added
 * to the queue is executed. When the time limit expires, another command is
 * added to the queue. If all the commands in the queue were executed before the
 * time expired then the cursor simply waits on the current sector.
 */

#pragma once

#include "common.h"

#define CMD_LENGTH	4		/* three characters + \0 */

struct hdd_queue {
	struct hdd_address *addr;
	struct hdd_queue *next;
	char cmd[CMD_LENGTH];
};

enum hdd_result hq_init(struct hdd_queue **q);

/* Extracts and adds a command from the input */
enum hdd_result hq_enqueue(struct hdd_queue **t, char *input); 

/* Removes a command */
struct hdd_queue *hq_dequeue(struct hdd_queue **h); 

/* Shows the current command */
struct hdd_queue *hq_peek(struct hdd_queue *h); 
