#include "hdd.h"
#include "common.h"

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
	int lines = 4;
	enum hdd_result r;
	FILE *in = NULL; 
	FILE *out = NULL;

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	r = hdd_print(&hdd);
	CHECK_RESULT(r);

	return EXIT_SUCCESS;
}
