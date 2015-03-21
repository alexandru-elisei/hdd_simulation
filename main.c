#include "hdd.h"
#include "types.h"

int main(int argc, char **argv)
{
	struct Sector *hdd;
	uint8_t lines = 0;

	hdd_init(&hdd, lines);

	return EXIT_SUCCESS;
}
