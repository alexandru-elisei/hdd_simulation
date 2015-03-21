#include "hdd.h"
#include "common.h"

#define CHECK_ERROR(r)					\
	do {						\
		if ((r) != HDD_SUCCESS)			\
			hdd_print_result((r));		\
		exit((r));				\
	} while (0)		

int main(int argc, char **argv)
{
	struct Sector *hdd;
	uint8_t lines = 0;
	hdd_result_t r;

	DEBMSG("test");

	DEBINFO(0 == 0);

	r = hdd_init(&hdd, lines);
	CHECK_ERROR(r);

	return EXIT_SUCCESS;
}
