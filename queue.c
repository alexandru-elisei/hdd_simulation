#include <string.h>

#include "queue.h"

static void dequeue(struct command_queue **q);

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

	new = (struct command_queue *) malloc(sizeof(struct command_queue));
	if (new == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	new->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (new->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	if (strncmp(buf, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0) {
		strncpy(new->cmd, COMMAND_EXIT, CMD_LENGTH);
		new->addr->line = -1;
		new->addr->index = -1;
	} else {
		tmp = strtok(buf, " ");
		strncpy(new->cmd, tmp, CMD_LENGTH);
		buf = buf + strlen(buf) + 1;
		tmp = strtok(buf, " ");
		sscanf(tmp, "%d", &(new->addr->line));
		buf = buf + strlen(buf) + 1;
		tmp = strtok(buf, " ");
		sscanf(tmp, "%d", &(new->addr->index));

		if (strncmp(new->cmd, COMMAND_WRITE, strlen(COMMAND_WRITE)) == 0) {
			buf = buf + strlen(buf) + 1;
			tmp = strtok(buf, "\n");
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
