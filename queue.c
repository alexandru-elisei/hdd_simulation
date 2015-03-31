#include <string.h>
#include <ctype.h>
#include <math.h>

#include "queue.h"

static void dequeue(struct command_queue **q);

static enum hdd_result enqueue_mread(struct command_queue **t,
			  struct command_queue **h,
			  char *buf);


static enum hdd_result enqueue_mwrite(struct command_queue **t,
			  struct command_queue **h,
			  char *buf);

void cq_init(struct command_queue **t, struct command_queue **h)
{
	*h = *t = NULL;
}
	
/* Adds a command to the queue */
enum hdd_result cq_enqueue(struct command_queue **t,
			   struct command_queue **h, 
			   char *buf)
{
	struct command_queue *new;
	char *tmp;

	DEBMSG(buf);
	if (strncmp(buf, COMMAND_MREAD, strlen(COMMAND_MREAD)) == 0)
		return enqueue_mread(t, h, buf);

	if (strncmp(buf, COMMAND_MWRITE, strlen(COMMAND_MREAD)) == 0)
		return enqueue_mwrite(t, h, buf);

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
		tmp = strtok(buf, " ");
		strcpy(new->cmd, tmp);
		tmp = strtok(NULL, " ");
		new->addr->line = atoi(tmp);
		tmp = strtok(NULL, " ");
		new->addr->index = atoi(tmp);

		if (strncmp(new->cmd, COMMAND_WRITE, strlen(COMMAND_WRITE)) == 0) {
			tmp = strtok(NULL, "\n");
			strncpy(new->data, tmp, SECTOR_SIZE);
		}
	}

	new->next = NULL;

	if (*h == NULL)
		*h = *t = new;
	else {
		(*t)->next = new;
		*t = new;
	}

	return HDD_SUCCESS;
}

/* Queues a multiple read command */
static enum hdd_result enqueue_mread(struct command_queue **t,
			  struct command_queue **h,
			  char *buf)
{
	struct command_queue *new;
	char *tmp;
	int start_line;
       	int start_index;
       	int number_of_reads;
	int queued_reads;		/* Number of reads queued so far */
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
	queued_reads = 0;
	/* The multiple read command is queued as individual read commands */
	while (FOREVER) {
		for (i = start_index; i < available_reads; i++) {
			new = (struct command_queue *) malloc(sizeof(struct command_queue));
			if (new == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
			if (new->addr == NULL)
				return HDD_ERROR_MEMORY_ALLOC;

			strncpy(new->cmd, "::r", 4);
			new->addr->line = start_line;
			new->addr->index = i;
 			new->next = NULL;

			if (*h == NULL)
				*h = *t = new;
			else {
				(*t)->next = new;
				*t = new;
			}

			queued_reads++;
			if (queued_reads == number_of_reads)
				return HDD_SUCCESS;
		}

		/* Moving to the line above the current one */
		start_index = 0;
		start_line++;
		available_reads = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	return HDD_ERROR_UNKNOWN_ERROR;
}

static enum hdd_result enqueue_mwrite(struct command_queue **t,
			  struct command_queue **h,
			  char *buf)
{
	struct command_queue *new;
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
		for (i = start_index; i < available_writes; i++) {
			tmp = strtok(NULL, " \n");
			if (tmp[0] == MWRITE_END_CHAR)
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
			strncpy(new->data, tmp, SECTOR_SIZE);
 			new->next = NULL;

			if (*h == NULL)
				*h = *t = new;
			else {
				(*t)->next = new;
				*t = new;
			}
		}

		/* Moving to the line above the current one */
		start_index = 0;
		start_line++;
		available_writes = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, start_line);
	}

	return HDD_ERROR_UNKNOWN_ERROR;
}

/* Executes a command */
enum hdd_result cq_execute(struct command_queue **head, 
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
	       dequeue(head);
	}

	return r;
}

static void dequeue(struct command_queue **q)
{
	struct command_queue *tmp;

	tmp = *q;
	(*q) = (*q)->next;
	free(tmp->addr);
	free(tmp);
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
	return (h == NULL);
}
