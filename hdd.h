#pragma once

#include <stdint.h>

#include "common.h"

#undef SECTOR_SIZE 
#define SECTOR_SIZE	5

/* Nonlinear linked list, each "level" is a circular list */
struct hdd_sector {
	struct hdd_sector *next;		/* Next sector in line */
	struct hdd_sector *below;		/* Previous line */
	struct hdd_sector *above;		/* Next line */
	char data[SECTOR_SIZE]; 	/* Data stored */
	unsigned int damage;
};

struct hdd_head {
	struct hdd_sector *sect;
	uint8_t line;
	unsigned int index;
}; 

/* Builds the harddrive */
enum hdd_result hdd_init(struct hdd_sector **s, const uint8_t lines);

/* The readhead is always initialized on sector 0 on line 0 */
enum hdd_result hdd_head_init(struct hdd_head **h);

/* Frees allocated space */
enum hdd_result hdd_dealocate(struct hdd_sector **s);

enum hdd_result hdd_head_dealocate(struct hdd_head *s);
