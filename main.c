#include <string.h>
#include <ctype.h>

#include "hdd.h"
#include "common.h"
#include "queue.h"
#include "stack.h"

#define STRLEN		(90000)
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
				hdd_destroy(&hdd);		\
			if (cursor != NULL)			\
				hdd_destroy_head(&cursor);	\
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

	int lines;				/* Drive number of lines */
	int option;				/* Stack or queue */
	int remaining_time;		
	int received_exit;			
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
	option = atoi(buffer);
	lines = atoi(buffer + 2);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_head_init(&cursor, hdd);
	CHECK_RESULT(r);

	fgets(buffer, STRLEN, in);
	if (option == QUEUE_OPTION) {
		cq_init(&cq_head, &cq_tail);
		r = cq_enqueue(&cq_head, &cq_tail, buffer, lines);
		CHECK_RESULT(r);	
	} else if (option == STACK_OPTION) {
		cs_init(&cs_top);
		r = cs_push(&cs_top, buffer, lines);
		CHECK_RESULT(r);	
	} else {
		CHECK_RESULT(HDD_ERROR_UNKNOWN_OPTION);
	}

	fgets(buffer, STRLEN, in);
	remaining_time = atoi(buffer);
	received_exit = 0;

if (option == QUEUE_OPTION) {
#ifdef DEBUG
	printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
			cq_tail->cmd, cq_tail->addr->line, 
			cq_tail->addr->index, remaining_time);
	elapsed_time = 0;
	printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
		elapsed_time, remaining_time, cursor->line,
		cursor->sect->index, cursor->sect->damage);
#endif
	while (FOREVER) {
		/* Reading a new command if time expired */
		while (remaining_time == 0 && received_exit == 0) {
			if (received_exit == 0) {
				fgets(buffer, STRLEN, in);
				if (strncmp(buffer, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0)
					received_exit = 1;

				r = cq_enqueue(&cq_head, &cq_tail, buffer, lines);
				CHECK_RESULT(r);
			}

			/* Reading allocated time if I haven't queued program exit */
			if (received_exit == 0) {
				fgets(buffer, STRLEN, in);
				remaining_time = atoi(buffer);
			}
#ifdef DEBUG
			if (cq_is_empty(cq_head) == 0)
				printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
				cq_tail->cmd, cq_tail->addr->line, 
				cq_tail->addr->index, remaining_time);
#endif
		}

		/* If I still have commands to execute */
		if (cq_is_empty(cq_head) == 0) {
			if (strncmp(COMMAND_EXIT, cq_head->cmd, strlen(COMMAND_EXIT)) == 0) {
				r = cq_destroy(&cq_head, &cq_tail);
				CHECK_RESULT(r);
				break;
			}

			r = hdd_seek(cq_head->addr, cursor);
			CHECK_RESULT(r);

			/* If I'm at the right address */
			if (r == HDD_SUCCESS) {
#ifdef DEBUG
				printf("EXECUTING: %s %d %d\n", 
					cq_head->cmd, cq_head->addr->line, 
					cq_head->addr->index);
#endif
				r = cq_execute(&cq_head, &cq_tail, cursor, out);
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
			elapsed_time, remaining_time, cursor->line,
			cursor->sect->index, cursor->sect->damage);
#endif
	}

} else {

#ifdef DEBUG
	printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
			cs_top->cmd, cs_top->addr->line, 
			cs_top->addr->index, remaining_time);
	elapsed_time = 0;
	printf("Elapsed:%4d|Remaining:%4d|(%4d, %4d)|Damage:%4d\n",
		elapsed_time, remaining_time, cursor->line,
		cursor->sect->index, cursor->sect->damage);
#endif
	while (FOREVER) {
		/* Reading a new command if time expired */
		while (remaining_time == 0 && received_exit == 0) {
			fgets(buffer, STRLEN, in);
			r = cs_push(&cs_top, buffer, lines);
			CHECK_RESULT(r);

			/*
			 * Reading allocated time if I haven't read program
			 * exit 
			 */
			if (strncmp(COMMAND_EXIT, buffer, strlen(COMMAND_EXIT)) != 0) {
				fgets(buffer, STRLEN, in);
				remaining_time = atoi(buffer);
			} else {
				received_exit = 1;
			}

#ifdef DEBUG
			if (cs_is_empty(cs_top) == 0)
				printf("-> COMMAND READ: %s %d %d, TIME READ: %d\n", 
				cs_top->cmd, cs_top->addr->line, 
				cs_top->addr->index, remaining_time);
#endif
		}

		/* If I read the exit command then that will get executed */
		if (received_exit == 1) {
			r = cs_destroy(&cs_top);
			CHECK_RESULT(r);
			break;
		}

		/* If I still have commands to execute */
		if (cs_is_empty(cs_top) == 0) {
			r = hdd_seek(cs_top->addr, cursor);
			CHECK_RESULT(r);

			if (r == HDD_SUCCESS) {
#ifdef DEBUG
				printf("EXECUTING: %s %d %d\n", 
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
			elapsed_time, remaining_time, cursor->line,
			cursor->sect->index, cursor->sect->damage);
#endif
	}
}	/* Stack or queue command execution */

	hdd_print_damage(hdd, out);

	/* Cleaning up */
	free(buffer);
	hdd_destroy(&hdd);
	hdd_destroy_head(&cursor);
	fclose(in);
	fclose(out);

	return EXIT_SUCCESS;
}
