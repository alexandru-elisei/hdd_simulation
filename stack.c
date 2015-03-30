#include <string.h>

#include "stack.h"

static void pop(struct command_stack **t);

void cs_init(struct command_stack **t)
{
	*t = NULL;
}
	
/* Adds a command to the stack */
enum hdd_result cs_push(struct command_stack **t,
			   char *buf)
{
	struct command_stack *new;
	char *tmp;

	new = (struct command_stack *) malloc(sizeof(struct command_stack));
	if (new == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (new->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	if (strncmp(buf, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0) {
		strncpy(new->cmd, COMMAND_EXIT, CMD_LENGTH);
		new->addr->line = -1;
		new->addr->index = -1;
		strncpy(new->data, "XXXX", SECTOR_SIZE);
	} else {
		tmp = strtok(buf, " ");
		strncpy(new->cmd, tmp, CMD_LENGTH);
		buf = buf + strlen(buf) + 1;
		tmp = strtok(buf, " ");
		sscanf(tmp, "%d", &(new->addr->line));
		buf = buf + strlen(buf) + 1;
		tmp = strtok(buf, " ");
		sscanf(tmp, "%d", &(new->addr->index));

		if (strcmp(new->cmd, COMMAND_WRITE) == 0) {
			buf = buf + strlen(buf) + 1;
			tmp = strtok(buf, "\n");
			strncpy(new->data, tmp, SECTOR_SIZE);
		} else {
			strncpy(new->data, "XXXX", SECTOR_SIZE);
		}
	}

	new->next = *t;
	*t = new;

	return HDD_SUCCESS;
}

/* Executes a command */
enum hdd_result cs_execute(struct command_stack **t, 
			struct hdd_head *h,
			FILE *out)
{
	enum hdd_result r;
	char output[SECTOR_SIZE];
	int write_to_file;

	if (*t == NULL || h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	write_to_file = 0;
	if (strcmp((*t)->cmd, COMMAND_READ) == 0) {
		r = hdd_read_data(h, output);
		write_to_file = 1;
	} else if (strcmp((*t)->cmd, COMMAND_WRITE) == 0) {
		r = hdd_write_data(h, (*t)->data);
	} else if (strcmp((*t)->cmd, COMMAND_DAMAGE) == 0) {
		r = hdd_read_damage(h, output);
		write_to_file = 1;
	}

	if (r == HDD_SUCCESS) {
	       if (write_to_file == 1)
			fprintf(out, "%s\n", output);
	       pop(t);
	}

	return r;
}

/* Dealocates the entire stack */
enum hdd_result cs_dealocate(struct command_stack **t)
{
	/* There's probably something wrong if I dealocate an empty stack */
	if (*t == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	while (*t != NULL)
		pop(t);

	return HDD_SUCCESS;
}

static void pop(struct command_stack **t)
{
	struct command_stack *tmp;

	tmp = *t;
	*t = (*t)->next;
	free(tmp->addr);
	free(tmp);
}

/* Prints the entire command queue */
void cs_print(struct command_stack *t)
{
	for (; t != NULL; t = t->next)
		printf("cmd = %s, data = %s, line = %d, index = %d\n\n",
			t->cmd, t->data, t->addr->line, t->addr->index);
}
		
/* Checks if no commands are prending */
int cs_is_empty(const struct command_stack *t)
{
	return (t == NULL);
}
