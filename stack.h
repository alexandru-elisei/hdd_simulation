/*
 * The stack for executing commands. The last command added to the stack is 
 * executed. When the time limit expires, another command is added to the stack
 * and that gets executed. If all the commands in the stack are executed 
 * before the time expired then the cursor idles on the current sector.
 */

#ifndef STACK_H				
#define STACK_H

#include <stdint.h>

#include "common.h"
#include "hdd.h"

struct command_stack {
	struct hdd_address *addr;	/* Address of the command */
	struct command_stack *next;	/* Next command in the stack */
	char cmd[CMD_LENGTH];		/* Literal text of the command */
	char data[SECTOR_SIZE];		/* Data to be written/read */
};

/* Initializes the stack */
void cs_init(struct command_stack **t);

/* Adds a command to the stack */
enum hdd_result cs_push(struct command_stack **t,
			   char *buf, int lines); 

/* Executes a command */
enum hdd_result cs_execute(struct command_stack **t,
			struct hdd_head *h,
			FILE *out);

/* Prints the entire command stack */
void cs_print(struct command_stack *t); 

/* Dealocates the entire stack */
enum hdd_result cs_destroy(struct command_stack **t);

/* Checks if no commands are prending */
int cs_is_empty(const struct command_stack *t);

#endif	/* #ifndef STACK_H */
