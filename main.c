#include <string.h>

#include "hdd.h"
#include "common.h"

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

int main(int argc, char **argv)
{
	struct hdd_sector *hdd = NULL;
	struct hdd_head *cursor = NULL;
	struct hdd_address *addr = NULL;
	int lines;
	int option;
	enum hdd_result r;
	FILE *in = NULL; 
	FILE *out = NULL;
	char *buffer;

	if (argc < 3)
		CHECK_RESULT(HDD_ERROR_INVALID_ARGUMENTS);

	if ((in = fopen(argv[1], "r")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	if ((out = fopen(argv[2], "w")) == NULL)
		CHECK_RESULT(HDD_ERROR_FILE_ACCESS);

	/* Reading program option */
	buffer = (char *) malloc(STRLEN * sizeof(char));
	fgets(buffer, STRLEN, in);
	sscanf(buffer, "%d", &option);
	buffer = buffer + 2;
	sscanf(buffer, "%d", &lines);

	DEBINFO(option);
	DEBINFO(lines);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_head_init(&cursor, hdd);
	CHECK_RESULT(r);

	addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if (addr == NULL)
		CHECK_RESULT(HDD_ERROR_MEMORY_ALLOC);

	addr->line = 0;
	addr->index = 3;

	while (hdd_seek(addr, cursor) != HDD_SEEK_SUCCESS)
		;

	addr->line = 0;
	addr->index = 2;

	while (hdd_seek(addr, cursor) != HDD_SEEK_SUCCESS)
		;

	hdd_print(hdd);

	return EXIT_SUCCESS;
}
