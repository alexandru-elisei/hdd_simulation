#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	(16)
#define MULTIPLY_FACTOR		(2)
#define DEFAULT_VALUE		("0000")
#define FOREVER			(1)

#undef pow
inline static int pow(int base, int exp);

/* Generates the hard drive */
enum hdd_result hdd_init(struct hdd_sector **s, int lines)
{
	struct hdd_sector *it;		/* iterator for the list */
	struct hdd_sector *index_0;	/* index 0 of current line */
	struct hdd_sector *new;		/* the new sector at each step */
	int i, j;
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
	for (i = 0; i < lines; i++) {
		req_sect = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, i);
		DEBINFO(req_sect);

		it = index_0;
		/* Creating current line as a circular linked list */
		for (j = 0; j < req_sect - 1; j++) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->below = new->above = NULL;
			it->next = new;
			new->next = index_0;
			it = new;
		}

		/* Creating the above line if necessary and linking it */
		if (lines > 1 && i < lines - 1) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->next = new;
			new->above = NULL;
			new->below = index_0;
			index_0->above = new;
			index_0 = new;
		}
	}

	return HDD_SUCCESS;
}

/* Prints the contents of the hard drive */
enum hdd_result hdd_print(struct hdd_sector **s)
{
	struct hdd_sector *it;
	struct hdd_sector *index_0;
	int line;

	if (*s == NULL)
		return HDD_ERROR_INVALID_RESOURCE;

	index_0 = *s;
	line = 0;
	while (FOREVER) {
		printf("LINE %d:\n", line);
		printf("\t%s", index_0->data);

		/* Printing the current line */
		for (it = index_0->next; it != index_0; it = it->next)
			printf(" %s", it->data);
		printf("\n");

		/* Moving to the above line, if it exists */
		if (index_0->above == NULL)
			break;
		else {
			index_0 = index_0->above;
			line++;
			DEBINFO(line);
		}
	}

	return HDD_SUCCESS;
}

	/*
	do {
		do {
			
		} while (it != index_0);
	} while (index->above != NULL);
	*/
	

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
