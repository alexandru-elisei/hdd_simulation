#pragma once

#include <stdint.h>

#include "types.h"

#undef SECTOR_SIZE 
#define SECTOR_SIZE	5

struct Sector {
	struct Sector *next;		/* Next sector in line */
	struct Sector *below;		/* Previous line */
	struct Sector *above;		/* Next line */
	char data[SECTOR_SIZE]; 	/* Data stored */
	unsigned int damage;
};

struct Hddhead {
	struct Sector *sect;
	uint8_t line;
	unsigned int index;
}; 

hdd_result_t hdd_init(struct Sector **s, const uint8_t lines);

hdd_result_t hdd_head_init(struct Hddhead **h);
