#include "hdd.h"
#include "common.h"

#define CHECK_RESULT(r)					\
	do { 						\
		if ((r) != HDD_SUCCESS)	{		\
			hdd_print_result((r));		\
			exit((r));			\
		}					\
	} while (0)		

int main(int argc, char **argv)
{
	struct hdd_sector *hdd;
	struct hdd_head *cursor;
	uint8_t lines = 0;
	enum hdd_result r;
	FILE *in, *out;

	DEBINFO(0 == 0);

	r = hdd_init(&hdd, lines);
	CHECK_RESULT(r);

	return EXIT_SUCCESS;
}
