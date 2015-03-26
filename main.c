#include <string.h>

#include "hdd.h"
#include "common.h"
#include "queue.h"

#define STRLEN		100

#define CHECK_RESULT(r)					\
	do { 						\
		if ((r) != HDD_SUCCESS)	{		\
			hdd_print_result((r));		\
			if (in != NULL)			\
				fclose(in);		\
			if (out != NULL)		\
				fclose(out);		\
			exit((r));			\
		}					\
	} while (0)		

/* Extracts the command and the address from the buffer */
void extract_command(char *b, char *cmd, struct hdd_address *a);

int main(int argc, char **argv)
{
	struct hdd_sector *hdd = NULL;	/* the hard drive */
	struct hdd_head *cursor = NULL;	/* the read/write head */
	struct hdd_address *addr = NULL;/* sector address */

	struct command_queue *head, *tail;	/* queue for commands */

	enum hdd_result r;
	FILE *in = NULL; 
	FILE *out = NULL;
	char *buffer;
	char cmd[CMD_STR_LENGTH];
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
	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &option);
	buffer = buffer + 2;
	sscanf(buffer, "%d", &lines);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_head_init(&cursor, hdd);
	CHECK_RESULT(r);

	addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (addr == NULL)
		CHECK_RESULT(HDD_ERROR_MEMORY_ALLOC);

	/*
	addr->line = 0;
	addr->index = 3;

	while (hdd_seek(addr, cursor) != HDD_SUCCESS)
		;

	addr->line = 0;
	addr->index = 2;

	while (hdd_seek(addr, cursor) != HDD_SUCCESS)
		;

	hdd_print(hdd);

	r = hdd_read_data(cursor, buffer);
	CHECK_RESULT(r);

	strcpy(buffer, "FFFF");
	r = hdd_write_data(cursor, buffer);
	CHECK_RESULT(r);

	hdd_print(hdd);

	hdd_read_damage(cursor, &damage);
	printf("\ndamage = %d\n", damage);
	*/

	cq_init(&tail, &head);

	fgets(buffer, STRLEN, in);
	extract_command(buffer, cmd, addr);

	DEBMSG(cmd);
	DEBINFO(addr->line);	
	DEBINFO(addr->index);	

	r = cq_enqueue(&tail, &head, addr, cmd);
	DEBMSG(tail->cmd);
	DEBMSG(head->cmd);
	DEBINFO(tail->addr->line);
	DEBINFO(tail->addr->index);

	return EXIT_SUCCESS;
}

/* Extracts the command and the address from the buffer */
void extract_command(char *b, char *cmd, struct hdd_address *a)
{
	char tmp[CMD_STR_LENGTH];
	int aux;

	strncpy(cmd, b, CMD_STR_LENGTH - 1);
	cmd[CMD_STR_LENGTH - 1] = '\0';

	b = b + CMD_STR_LENGTH;
	sscanf(b, "%d", &aux);
	snprintf(tmp, CMD_STR_LENGTH, "%d", aux);
	a->line = aux;

	b = b + strlen(tmp) + 1;
	sscanf(b, "%d", &aux);
	snprintf(tmp, CMD_STR_LENGTH, "%d", aux);
	a->index = aux;
}
