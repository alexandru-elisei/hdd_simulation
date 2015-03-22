#pragma once

#include <stdint.h>

#include "common.h"

#undef SECTOR_SIZE 
#define SECTOR_SIZE	5

/* Nonlinear linked list, each "level" is a circular list */
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

enum hdd_result hdd_init(struct Sector **s, const uint8_t lines);

enum hdd_result hdd_head_init(struct Hddhead **h);
