#include <string.h>
#include <math.h>

#include "stack.h"

static enum hdd_result push_mread(struct command_stack **t,
			  char *buf);


static enum hdd_result push_mwrite(struct command_stack **t,
			  char *buf);

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

	if (strncmp(buf, COMMAND_MREAD, strlen(COMMAND_MREAD)) == 0)
		return push_mread(t, buf);

	if (strncmp(buf, COMMAND_MWRITE, strlen(COMMAND_MREAD)) == 0)
		return push_mwrite(t, buf);

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
		strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
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
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
		}
	}

	new->next = *t;
	*t = new;

	return HDD_SUCCESS;
}

/* Adds a multiple read command to the stack */
static enum hdd_result push_mread(struct command_stack **t,
			  char *buf)
{
	struct command_stack *new;
	char *tmp;
	int start_line;
       	int start_index;
       	int number_of_reads;
	int pushed_reads;		/* Number of reads pushed so far */
	int available_reads;		/* Sectors to read on current line */
	int i;

	buf = buf + strlen(COMMAND_MREAD) + 1;
	tmp = strtok(buf, " ");
	start_line = atoi(tmp);
	tmp = strtok(NULL, " ");
	start_index = atoi(tmp);
	tmp = strtok(NULL, "\n");
	number_of_reads = atoi(tmp);

	available_reads = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	pushed_reads = 0;
	/* The multiple read command is queued as individual read commands */
	while (FOREVER) {
		for (i = 0; i < available_reads; i++) {
			new = (struct command_stack *) malloc(sizeof(struct command_stack));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::r", 4);
			new->addr->line = start_line;
			new->addr->index = (i + start_index) % available_reads;

			new->next = *t;
			*t = new;

			pushed_reads++;
			if (pushed_reads == number_of_reads)
				return HDD_SUCCESS;
		}

		/* Moving to the line above the current one */
		start_index = 0;
		start_line++;
		available_reads = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	return HDD_ERROR_UNKNOWN_ERROR;
}

static enum hdd_result push_mwrite(struct command_stack **t,
			  char *buf)
{
	struct command_stack *new;
	char *tmp;
	int start_line;
       	int start_index;
	int available_writes;		/* Sectors to writes on current line */
	int i;

	buf = buf + strlen(COMMAND_MWRITE) + 1;
	tmp = strtok(buf, " ");
	start_line = atoi(tmp);
	tmp = strtok(NULL, " ");
	start_index = atoi(tmp);

	available_writes = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	while (FOREVER) {
		for (i = 0; i < available_writes; i++) {
			tmp = strtok(NULL, " \n");
			if (tmp[0] == MWRITE_END_CHAR)
				return HDD_SUCCESS;

			new = (struct command_stack *) malloc(sizeof(struct command_stack));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::w", 4);
			new->addr->line = start_line;
			new->addr->index = (i + start_index) % available_writes;
			strncpy(new->data, tmp, SECTOR_SIZE);

			new->next = *t;
			*t = new;
		}

		/* Moving to the line above the current one */
		start_index = 0;
		start_line++;
		available_writes = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	return HDD_ERROR_UNKNOWN_ERROR;
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
