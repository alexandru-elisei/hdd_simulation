#include <string.h>
#include <ctype.h>
#include <math.h>

#include "queue.h"

#define ENQUEUE(head, tail, new)			\
	do {						\
		(new)->next = NULL;			\
		if ((tail) == NULL)			\
			(tail) = (head) = (new);	\
		else {					\
			(tail)->next = (new);		\
			(tail) = (new);			\
		}					\
	} while (0)

/* Removes a command from the head of the queue */
inline static struct command_queue *dequeue(struct command_queue **h,
					struct command_queue **t);

/* Queues a multiple read command as individual read commands */
static enum hdd_result enqueue_mread(struct command_queue **t,
			  struct command_queue **h,
			  char *buf, int lines);

/* Queues a multiple write command as individual write commands */
static enum hdd_result enqueue_mwrite(struct command_queue **t,
			  struct command_queue **h,
			  char *buf, int lines);

/* Initializes the queue */
void cq_init(struct command_queue **h, struct command_queue **t)
{
	*h = *t = NULL;
}
	
/* Adds a command to the queue */
enum hdd_result cq_enqueue(struct command_queue **h,
			   struct command_queue **t, 
			   char *buf, int lines)
{
	struct command_queue *new;
	char *input;

	if (strncmp(buf, COMMAND_MREAD, strlen(COMMAND_MREAD)) == 0)
		return enqueue_mread(t, h, buf, lines);

	if (strncmp(buf, COMMAND_MWRITE, strlen(COMMAND_MWRITE)) == 0)
		return enqueue_mwrite(t, h, buf, lines);

	new = (struct command_queue *) malloc(sizeof(struct command_queue));
	if (new == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (new->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	if (strncmp(buf, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0) {
		strcpy(new->cmd, COMMAND_EXIT);
		new->addr->line = -1;
		new->addr->index = -1;
	} else {
		input = strtok(buf, " ");
		strcpy(new->cmd, input);
		input = strtok(NULL, " ");
		new->addr->line = atoi(input);
		input = strtok(NULL, " \r\n");
		new->addr->index = atoi(input);

		if (strncmp(new->cmd, COMMAND_WRITE, strlen(COMMAND_WRITE)) == 0) {
			input = strtok(NULL, " \r\n");
			strncpy(new->data, input, SECTOR_SIZE);
		}
	}

	ENQUEUE(*h, *t, new);

	return HDD_SUCCESS;
}

/* Queues a multiple read command as individual read commands */
static enum hdd_result enqueue_mread(struct command_queue **t,
			   struct command_queue **h,
			   char *buf, int lines)
{
	struct command_queue *new;
	char *input;
	int start_line;
       	int start_index;
       	int number_of_reads;
	int queued_reads;		/* Number of reads queued so far */
	int sectors;			/* Sectors to read on current line */
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
	queued_reads = 0;
	if (lines == 1)
		/* Not leaving the first line if I only have one line */
		direction = 0;
	else
		/* Reading upwards by default */
		direction = 1;
	/* The multiple read command is queued as individual read commands */
	while (FOREVER) {
		for (i = start_index; i < sectors; i++) {
			new = (struct command_queue *) malloc(sizeof(struct command_queue));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::r", 4);
			new->addr->line = start_line;
			new->addr->index = i;

			ENQUEUE(*h, *t, new);

			queued_reads++;
			if (queued_reads == number_of_reads)
				return HDD_SUCCESS;
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

	return HDD_ERROR_UNKNOWN_ERROR;
}

/* Queues a multiple write command as individual write commands */
static enum hdd_result enqueue_mwrite(struct command_queue **t,
			  struct command_queue **h,
			  char *buf, int lines)
{
	struct command_queue *new;
	char *input;
	int start_line;
       	int start_index;
	int sectors;		/* Sectors to writes on current line */
	int direction;
	int i;

	buf = buf + strlen(COMMAND_MWRITE) + 1;
	input = strtok(buf, " ");
	start_line = atoi(input);
	input = strtok(NULL, " ");
	start_index = atoi(input);

	sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	if (lines == 1)
		direction = 0;
	else
		direction = 1;
	while (FOREVER) {
		for (i = start_index; i < sectors; i++) {
			input = strtok(NULL, " \n");
			if (input[0] == MWRITE_END_CHAR)
				return HDD_SUCCESS;

			new = (struct command_queue *) malloc(sizeof(struct command_queue));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::w", 4);
			new->addr->line = start_line;
			new->addr->index = i;
			strncpy(new->data, input, SECTOR_SIZE);

			ENQUEUE(*h, *t, new);
		}

		/* Moving to the line above the current one */
		start_index = 0;
		if (lines > 1) {
			if (start_line == lines - 1)
				direction = -1;
			else if (start_line == 0)
				direction = 1;
		}
		start_line += direction;
		sectors = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	return HDD_ERROR_UNKNOWN_ERROR;
}

/* Executes a command */
enum hdd_result cq_execute(struct command_queue **head, 
				struct command_queue **tail,
			struct hdd_head *h,
			FILE *out)
{
	enum hdd_result r;
	char output[SECTOR_SIZE];
	int write_to_file;

	if (*head == NULL || h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	write_to_file = 0;
	if (strcmp((*head)->cmd, COMMAND_READ) == 0) {
		r = hdd_read_data(h, output);
		write_to_file = 1;
	} else if (strcmp((*head)->cmd, COMMAND_WRITE) == 0) {
		r = hdd_write_data(h, (*head)->data);
	} else if (strcmp((*head)->cmd, COMMAND_DAMAGE) == 0) {
		r = hdd_read_damage(h, output);
		write_to_file = 1;
	}

	if (r == HDD_SUCCESS) {
	       if (write_to_file == 1)
			fprintf(out, "%s\n", output);
	       free(dequeue(head, tail));
	}

	return r;
}

/* Frees the memory occupied by the queue */
enum hdd_result cq_destroy(struct command_queue **h,
		struct command_queue **t)
{
	struct command_queue *tmp;

	if (*h == NULL || *t == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	while (*h != NULL) {
		tmp = dequeue(h, t);
		free(tmp->addr);
		free(tmp);
	}

	return HDD_SUCCESS;
}

/* Removes a command from the head of the queue */
inline static struct command_queue *dequeue(struct command_queue **h,
					struct command_queue **t)
{
	struct command_queue *tmp;

	tmp = *h;
	(*h) = (*h)->next;
	if (*h == NULL)
		*t = NULL;

	return tmp;
}

/* Prints the entire command queue */
void cq_print(struct command_queue *h)
{
	for (; h != NULL; h = h->next) {
		if (strncmp(h->cmd, COMMAND_WRITE, strlen(COMMAND_WRITE)) == 0)
			printf("cmd = %s, data = %s, line = %d, index = %d\n",
				h->cmd, h->data, h->addr->line, h->addr->index);
		else if (strncmp(h->cmd, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0)
			printf("cmd = %s\n", h->cmd);
		else
			printf("cmd = %s, line = %d, index = %d\n",
				h->cmd, h->addr->line, h->addr->index);
	}
}
		
/* Checks if no commands are prending */
int cq_is_empty(const struct command_queue *h)
{
	if (h == NULL)
		return 1;
	else
		return 0;
}
