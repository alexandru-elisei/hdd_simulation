#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	16
#define MULTIPLY_FACTOR		2
#define DEFAULT_VALUE		"0000"

#undef pow
inline static int pow(int base, int exp);

/* Generates the harddrive */
enum hdd_result hdd_init(struct hdd_sector **s, int lines)
{
	struct hdd_sector *it;		/* iterator for the list */
	struct hdd_sector *index_0;	/* index 0 of current line */
	struct hdd_sector *new;		/* the new sector at each step */
	int i;
       	int line_num;			/* current line number */
       	int req_sect;			/* required sectors per line */

	if (lines <= 0)
		return HDD_ERROR_INVALID_PARAMETER;

	*s = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));

	if (*s == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*s)->below = (*s)->above = NULL;
	(*s)->next = *s;
	strncpy((*s)->data, DEFAULT_VALUE, SECTOR_SIZE);

	index_0 = *s;
	for (line_num = 0; line_num < lines; line_num++) {
		req_sect = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, line_num);
		DEBINFO(req_sect);

		it = index_0;
		/* Creating current line as a circular linked list */
		for (i = 0; i < req_sect; i++) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->below = new->above = NULL;
			it->next = new;
			new->next = index_0;
			it = new;
		}

		/* 
		 * Creating sector with index_0 on the next line 
		 * and advancing to the nect line
		 */
		new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
		strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
		new->next = new;
		new->above = NULL;
		new->below = index_0;
		index_0 = new;
	}

	return HDD_SUCCESS;
}

/* The drive head is always initialized on sector 0 on line 0 */
enum hdd_result hdd_head_init(struct hdd_head **h)
{
	*h = (struct hdd_head *) malloc(sizeof(struct hdd_head));
	(*h)->sect = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
	(*h)->line = 0;
	(*h)->index = 0;

	return HDD_SUCCESS;
}

inline static int pow(int base, int exp)
{
	int res;
	int i;

	if (exp == 0)
		return 1;

	res = base;
	for (i = 1; i < exp; i++)
		res = res * base;

	return res;
}
