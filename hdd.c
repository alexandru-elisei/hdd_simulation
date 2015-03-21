#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	16
#define MULTIPLY_FACTOR		2

hdd_result_t hdd_init(struct Sector **s, uint8_t lines)
{
	return HDD_ERROR_INVALID_PARAMETER;
}

hdd_result_t hdd_head_init(struct Hddhead **h)
{
	*h = (struct Hddhead *) malloc(sizeof(struct Hddhead));
	(*h)->sect = (struct Sector *) malloc(sizeof(struct Sector));
	(*h)->line = 0;
	(*h)->index = 0;

	return HDD_SUCCESS;
}

