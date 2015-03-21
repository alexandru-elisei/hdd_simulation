#pragma once

#undef SECTOR_SIZE 
#define SECTOR_SIZE	5

struct Sector {
	struct Sector *next;		/* Next sector in line */
	struct Sector *below;		/* Previous line */
	struct Sector *above;		/* Next line */
	char data[SECTOR_SIZE]; 	/* Data stored */
};
