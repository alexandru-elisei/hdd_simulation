#include <string.h>

#include "hdd.h"
#include "common.h"
#include "queue.h"

#define STRLEN		100
#define FOREVER		1

#define CHECK_RESULT(r)					\
	do { 						\
		if ((r) != HDD_SUCCESS &&		\
		(r) != SEEK_INCOMPLETE)	{		\
			hdd_print_result((r));		\
			if (in != NULL)			\
				fclose(in);		\
			if (out != NULL)		\
				fclose(out);		\
			exit((r));			\
		}					\
	} while (0)		

int main(int argc, char **argv)
{
	struct hdd_sector *hdd = NULL;	/* the hard drive */
	struct hdd_head *cursor = NULL;	/* the read/write head */

	struct command_queue *cq_head, *cq_tail;	/* queue for commands */

	enum hdd_result r;
	FILE *in = NULL; 
	FILE *out = NULL;
	char *buffer;
	char cmd[CMD_LENGTH];
	char output[SECTOR_SIZE];
	int lines;			/* number of lines the drive has */
	int option;			/* if we are using a stack or queue */
	int damage;			/* damage on a sector */
	int time_limit;			/* maximum time for a command */
	int remaining_time;		

	if (argc < 3)
		CHECK_RESULT(HDD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	/* Reading program options */
	buffer = (char *) malloc(STRLEN * sizeof(char));
	/*
	 * Ni se garanteaza corectitudinea fisierului de input, deci nu mai fac
	 * verificari legate de EOF
	 */
	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &option);
	buffer = buffer + 2;
	sscanf(buffer, "%d", &lines);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_head_init(&cursor, hdd);
	CHECK_RESULT(r);

	cq_init(&cq_tail, &cq_head);

	fgets(buffer, STRLEN, in);
	r = cq_enqueue(&cq_tail, &cq_head, buffer);
	CHECK_RESULT(r);	

	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &time_limit);
	remaining_time = time_limit;

	while(FOREVER) {
		if (cq_is_empty(cq_head) == 0) {
			r = hdd_seek(cq_head->addr, cursor);
			CHECK_RESULT(r);

			if (r == HDD_SUCCESS) {
				r = cq_execute(&cq_head, cursor, output);
				CHECK_RESULT(r);
			}
		} else {
			if (remaining_time == 0);
			/* hdd_idle */
		}
				
	}

	r = cq_execute(&cq_head, cursor, output);
	CHECK_RESULT(r);

	fgets(buffer, STRLEN, in);
	r = cq_enqueue(&cq_tail, &cq_head, buffer);
	CHECK_RESULT(r);	

	cq_print(cq_head);

	r = cq_execute(&cq_head, cursor, output);
	CHECK_RESULT(r);

	DEBMSG(output);
	
	/*
	DEBMSG(buffer);
	DEBMSG(cq_tail->cmd);
	DEBMSG(cq_tail->data);
	DEBINFO(cq_tail->addr->line);
	DEBINFO(cq_tail->addr->index);

	DEBMSG(cq_head->cmd);
	DEBINFO(cq_head->addr->line);
	DEBINFO(cq_head->addr->index);
	*/

	return EXIT_SUCCESS;
}
