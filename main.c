#include <string.h>

#include "hdd.h"
#include "common.h"
#include "queue.h"
#include "stack.h"

#define STRLEN		(100)
#define QUEUE_OPTION	(1)
#define STACK_OPTION	(2)

#define CHECK_RESULT(r)						\
	do { 							\
		if ((r) != HDD_SUCCESS &&			\
		(r) != HDD_SEEK_INCOMPLETE)	{		\
			hdd_print_result((r));			\
			if (in != NULL)				\
				fclose(in);			\
			if (out != NULL)			\
				fclose(out);			\
			if (buffer != NULL)			\
				free(buffer);			\
			if (hdd != NULL)			\
				hdd_dealocate(hdd);		\
			if (cursor != NULL)			\
				hdd_dealocate_head(cursor);	\
			exit((r));				\
		}						\
	} while (0)		

int main(int argc, char **argv)
{
	struct hdd_sector *hdd = NULL;		/* The hard drive */
	struct hdd_head *cursor = NULL;		/* The read/write head */
	struct command_queue *cq_head, *cq_tail;/* Queue for commands */
	struct command_stack *cs_top;		/* Stack for commands */
	enum hdd_result r;

	FILE *in = NULL; 			/* Input/output */
	FILE *out = NULL;
	char *buffer = NULL;

	char *aux;
	int lines;				/* Drive number of lines */
	int option;				/* Stack or queue */
	int remaining_time;		
#ifdef DEBUG
	int elapsed_time;
#endif

	if (argc < 3)
		CHECK_RESULT(HDD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	/* Reading program options */
	buffer = (char *) malloc(STRLEN * sizeof(char));
	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &option);
	aux = buffer + 2;
	sscanf(aux, "%d", &lines);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_head_init(&cursor, hdd);
	CHECK_RESULT(r);

	fgets(buffer, STRLEN, in);
	if (option == QUEUE_OPTION) {
		cq_init(&cq_tail, &cq_head);
		r = cq_enqueue(&cq_tail, &cq_head, buffer);
		CHECK_RESULT(r);	
	} else if (option == STACK_OPTION) {
		cs_init(&cs_top);
		r = cs_push(&cs_top, buffer);
		CHECK_RESULT(r);	
	} else {
		CHECK_RESULT(HDD_ERROR_UNKNOWN_OPTION);
	}

	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &remaining_time);

if (option == QUEUE_OPTION) {
#ifdef DEBUG
	printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
			cq_tail->cmd, cq_tail->addr->line, 
			cq_tail->addr->index, remaining_time);
	elapsed_time = 0;
	printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
		elapsed_time, remaining_time, cursor->addr->line,
		cursor->addr->index, cursor->sect->damage);
#endif
	while(FOREVER) {
		/* Reading a new command if time expired */
		if (remaining_time == 0) {
			fgets(buffer, STRLEN, in);
			r = cq_enqueue(&cq_tail, &cq_head, buffer);
			CHECK_RESULT(r);

			/* Reading allocated time if I haven't queued program exit */
			if (strncmp(COMMAND_EXIT, cq_tail->cmd, strlen(COMMAND_EXIT)) != 0) {
				fgets(buffer, STRLEN, in);
				sscanf(buffer, "%d", &remaining_time);
			}
#ifdef DEBUG
			printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
				cq_tail->cmd, cq_tail->addr->line, 
				cq_tail->addr->index, remaining_time);
#endif
		}

		/* If I still have commands to execute */
		if (cq_is_empty(cq_head) == 0) {
			if (strncmp(COMMAND_EXIT, cq_head->cmd, strlen(COMMAND_EXIT)) == 0)
				break;

			r = hdd_seek(cq_head->addr, cursor);
			CHECK_RESULT(r);

			if (r == HDD_SUCCESS) {
#ifdef DEBUG
				printf("EXECUTED: %s %d %d\n", 
					cq_head->cmd, cq_head->addr->line, 
					cq_head->addr->index);
#endif
				r = cq_execute(&cq_head, cursor, out);
				CHECK_RESULT(r);
			}
		/* Command queue is indeed empty */
		} else {
			hdd_idle(cursor);
		}

		--remaining_time;
#ifdef DEBUG
		elapsed_time++;
		printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
			elapsed_time, remaining_time, cursor->addr->line,
			cursor->addr->index, cursor->sect->damage);
#endif
	}

} else {

#ifdef DEBUG
	printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
			cs_top->cmd, cs_top->addr->line, 
			cs_top->addr->index, remaining_time);
	elapsed_time = 0;
	printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
		elapsed_time, remaining_time, cursor->addr->line,
		cursor->addr->index, cursor->sect->damage);
#endif
	while(FOREVER) {
		/* Reading a new command if time expired */
		if (remaining_time == 0) {
			fgets(buffer, STRLEN, in);
			r = cs_push(&cs_top, buffer);
			CHECK_RESULT(r);

			/*
			 * Reading allocated time if I haven't read program
			 * exit 
			 */
			if (strncmp(COMMAND_EXIT, buffer, strlen(COMMAND_EXIT)) != 0) {
				fgets(buffer, STRLEN, in);
				sscanf(buffer, "%d", &remaining_time);
			}

#ifdef DEBUG
			printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
				cs_top->cmd, cs_top->addr->line, 
				cs_top->addr->index, remaining_time);
#endif
		}

		/* If I still have commands to execute */
		if (cs_is_empty(cs_top) == 0) {
			if (strncmp(COMMAND_EXIT, cs_top->cmd, strlen(COMMAND_EXIT)) == 0) {
				cs_dealocate(&cs_top);
				break;
			}

			r = hdd_seek(cs_top->addr, cursor);
			CHECK_RESULT(r);

			if (r == HDD_SUCCESS) {
#ifdef DEBUG
				printf("EXECUTED: %s %d %d\n", 
					cs_top->cmd, cs_top->addr->line, 
					cs_top->addr->index);
#endif
				r = cs_execute(&cs_top, cursor, out);
				CHECK_RESULT(r);
			}
		/* Command queue is indeed empty */
		} else {
			hdd_idle(cursor);
		}

		--remaining_time;
#ifdef DEBUG
		elapsed_time++;
		printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
			elapsed_time, remaining_time, cursor->addr->line,
			cursor->addr->index, cursor->sect->damage);
#endif
	}
}	/* Stack or queue command execution */

	hdd_print_damage(hdd, out);

	/* Cleaning up */
	free(buffer);
	hdd_dealocate(hdd);
	hdd_dealocate_head(cursor);
	fclose(in);
	fclose(out);

	return EXIT_SUCCESS;
}
