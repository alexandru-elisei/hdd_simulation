#ifndef HDD_H
#define HDD_H

#include "common.h"
#include <stdint.h>

/* 
 * Nonlinear linked list, each "level" is a circular list linked to the
 * previous and above "level" at item with index 0
 */
struct hdd_sector {
	struct hdd_sector *next;		/* Next sector in line */
	struct hdd_sector *below;		/* Previous line */
	struct hdd_sector *above;		/* Next line */
	char data[SECTOR_SIZE]; 		/* Data stored */
	unsigned int damage;
	uint8_t is_index_0;			/* If the sector had index 0 */
};

/* Drive head, reads the current sector and stores the position on the drive */
struct hdd_head {
	struct hdd_sector *sect;
	struct hdd_address *addr;
}; 

/* Generates the hard drive */
enum hdd_result hdd_init(struct hdd_sector **s, int lines);

/* Prints the contents of the hard drive */
enum hdd_result hdd_print(struct hdd_sector *s);

/* The drive head is always initialized on sector 0 on line 0 */
enum hdd_result hdd_head_init(struct hdd_head **h, struct hdd_sector *s);

/* 
 * Seeks an address by jumping one sector at a time.
 * Each time I call it I add cursor damage
 */
enum hdd_result hdd_seek(struct hdd_address *a, struct hdd_head *h);

/* The drive head idles at a location */
enum hdd_result hdd_idle(struct hdd_head *h);

/* Reads data from the current sector */
enum hdd_result hdd_read_data(struct hdd_head *h, char *data);

/* Writes data from the current sector */
enum hdd_result hdd_write_data(struct hdd_head *h, char *data);

/* Read damage data from the current sector */
enum hdd_result hdd_read_damage(struct hdd_head *h, char *damage);

/* Frees allocated drive memory */
enum hdd_result hdd_dealocate(struct hdd_sector *s);

/* Frees allocated drive head memory */
enum hdd_result hdd_dealocate_head(struct hdd_head *h);

/* Calculate average damage across the hard drive */
enum hdd_result hdd_print_damage(struct hdd_sector *h, FILE *out);

#endif	/* #ifndef HDD_H */
