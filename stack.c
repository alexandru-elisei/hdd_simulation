#include <string.h>
#include <math.h>

#include "stack.h"

#define PUSH(top, new)				\
	do {					\
		(new)->next = (top);		\
		(top) = (new);			\
	} while (0)

/* Adds a multiple read command as individual read commands */
static enum hdd_result push_mread(struct command_stack **t,
			  char *buf, int lines);

/* Adds a multiple write command as individual write commands */
static enum hdd_result push_mwrite(struct command_stack **t,
			  char *buf, int lines);

/* Returns the current element of the stack */
static struct command_stack *pop(struct command_stack **t);

/* Initializes the stack */
void cs_init(struct command_stack **t)
{
	*t = NULL;
}
	
/* Adds a command to the stack */
enum hdd_result cs_push(struct command_stack **t,
			   char *buf, int lines)
{
	struct command_stack *new;
	char *input;

	if (strncmp(buf, COMMAND_MREAD, strlen(COMMAND_MREAD)) == 0)
		return push_mread(t, buf, lines);

	if (strncmp(buf, COMMAND_MWRITE, strlen(COMMAND_MWRITE)) == 0)
		return push_mwrite(t, buf, lines);

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
		input = strtok(buf, " ");
		strncpy(new->cmd, input, CMD_LENGTH);
		buf = buf + strlen(buf) + 1;
		input = strtok(buf, " ");
		sscanf(input, "%d", &(new->addr->line));
		buf = buf + strlen(buf) + 1;
		input = strtok(buf, " \r\n");
		sscanf(input, "%d", &(new->addr->index));

		if (strcmp(new->cmd, COMMAND_WRITE) == 0) {
			buf = buf + strlen(buf) + 1;
			input = strtok(buf, " \r\n");
			strncpy(new->data, input, SECTOR_SIZE);
		} else {
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
		}
	}

	PUSH(*t, new);

	return HDD_SUCCESS;
}

/* Adds a multiple read command as individual read commands */
static enum hdd_result push_mread(struct command_stack **t,
			   char *buf, int lines)
{
	struct command_stack *new, *tmp;
	char *input;
	int start_line;
       	int start_index;
       	int number_of_reads;
	int pushed_reads;	/* Number of reads pushed so far */
	int sectors;		/* Numbers of sectors on current line */
	int direction;
	int i;

	buf = buf + strlen(COMMAND_MREAD) + 1;
	input = strtok(buf, " ");
	start_line = atoi(input);
	input = strtok(NULL, " ");
	start_index = atoi(input);
	input = strtok(NULL, "\n");
	number_of_reads = atoi(input);

	sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	pushed_reads = 0;
	tmp = NULL;
	if (lines == 1)
		/* Not leaving the first line if I only have one line */
		direction = 0;
	else
		/* Reading upwards by default */
		direction = 1;
	/* 
	 * The multiple read command is queued as individual read commands. The
	 * individual read commands are first pushed into a temporary stack,
	 * popped and pushed into the command stack, so the first command read,
	 * first pushed in the temporary stack, is now last in the command
	 * stack, therefore first to be executed
	 */
	while (pushed_reads < number_of_reads) {
		for (i = start_index; i < sectors; i++) {
			new = (struct command_stack *) malloc(sizeof(struct command_stack));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::r", 4);
			new->addr->line = start_line;
			new->addr->index = i;

			PUSH(tmp, new);

			pushed_reads++;
			if (pushed_reads == number_of_reads)
				break;
		}

		start_index = 0;
		/* Choosing the direction to move in next */
		if (lines > 1) {
			if (start_line == lines - 1)
				direction = -1;
			else if (start_line == 0)
				direction = 1;
		}
		start_line += direction;
		sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	/* Emptying the secondary stack into the command stack */
	while (tmp != NULL) {
		new = pop(&tmp);
		PUSH(*t, new);
	}

	return HDD_SUCCESS;
}

/* Adds a multiple write command as individual write commands */
static enum hdd_result push_mwrite(struct command_stack **t,
			  char *buf, int lines)
{
	struct command_stack *new, *tmp;
	char *input;
	int start_line;
       	int start_index;
	int sectors;		/* Sectors to writes on current line */
	int direction;
	int i;
	int done;

	buf = buf + strlen(COMMAND_MWRITE) + 1;
	input = strtok(buf, " ");
	start_line = atoi(input);
	input = strtok(NULL, " ");
	start_index = atoi(input);

	sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	done = 0;
	tmp = NULL;
	if (lines == 1)
		/* Not leaving the first line if I only have one line */
		direction = 0;
	else
		/* Reading upwards by default */
		direction = 1;
	/* 
	 * The multiple write command is queued as individual write commands.
	 * Individual write commands are first pushed into a temporary stack,
	 * then popped into the command stack, so the first command read,
	 * first pushed in the temporary stack, is now last in the command
	 * stack, therefore first to be executed
	 */
	while (done == 0) {
		for (i = start_index; i < sectors; i++) {
			input = strtok(NULL, " \n");
			if (input[0] == MWRITE_END_CHAR) {
				done = 1;
				break;
			}

			new = (struct command_stack *) malloc(sizeof(struct command_stack));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::w", 4);
			new->addr->line = start_line;
			new->addr->index = i;
			strncpy(new->data, input, SECTOR_SIZE);

			PUSH(tmp, new);
		}

		start_index = 0;
		/* Choosing the direction to move in next */
		if (lines > 1) {
			if (start_line == lines - 1)
				direction = -1;
			else if (start_line == 0)
				direction = 1;
		}
		start_line += direction;
		sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	/* Emptying the temporary stack into the command stack */
	while (tmp != NULL) {
		new = pop(&tmp);
		PUSH(*t, new);
	}

	return HDD_SUCCESS;
}

/* Executes a command */
enum hdd_result cs_execute(struct command_stack **t, 
			struct hdd_head *h,
			FILE *out)
{
	struct command_stack *tmp;
	enum hdd_result r;
	char output[SECTOR_SIZE];
	int write_to_file;

	if (*t == NULL || h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	write_to_file = 0;
	/* Recognizing the command */
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
	       tmp = pop(t);
	       free(tmp->addr);
	       free(tmp);
	}

	return r;
}

/* Dealocates the entire stack */
enum hdd_result cs_destroy(struct command_stack **t)
{
	/* There's probably something wrong if I dealocate an empty stack */
	struct command_stack *tmp;

	if (*t == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	while (*t != NULL) {
		tmp = pop(t);
		free(tmp->addr);
		free(tmp);
	}

	return HDD_SUCCESS;
}

/* Returns the current element of the stack */
static struct command_stack *pop(struct command_stack **t)
{
	struct command_stack *tmp;

	tmp = *t;
	*t = (*t)->next;

	return tmp;
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
