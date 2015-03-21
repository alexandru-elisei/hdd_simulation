#pragma once

#undef SECTOR_SIZE 
#define SECTOR_SIZE	5

struct Sector {
	struct Sector *next;
	struct Sector *below;
	struct Sector *above;
	char val[SECTOR_SIZE]; 
};
