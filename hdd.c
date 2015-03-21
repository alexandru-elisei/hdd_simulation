#include "hdd.h"

#include <assert.h>

#define INITIAL_LINE_LENGTH	16
#define STEP			2

int hdd_init(struct Sector **s, uint8_t lines)
{
	
	return 1;
}

int hdd_head_init(struct Hddhead **h)
{
	*h = (struct Hddhead *) malloc(sizeof(struct Hddhead));
	(*h)->sect = (struct Sector *) malloc(sizeof(struct Sector));
	(*h)->line = 0;
	(*h)->index = 0;

	return 1;
}

