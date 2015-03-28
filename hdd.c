#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	(16)
#define MULTIPLY_FACTOR		(2)
#define DEFAULT_VALUE		("0000")
#define FOREVER			(1)
#define READ_DATA_DAMAGE	(5)
#define WRITE_DAMAGE		(30)
#define READ_DAMAGE_DAMAGE	(2)
#define CURSOR_DAMAGE		(1)

#undef pow
inline static int pow(int base, int exp);
inline static void add_damage(struct hdd_head *h, int damage);

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
	(*s)->damage = 0;

	index_0 = *s;
	for (i = 0; i < lines; i++) {
		req_sect = INITIAL_LINE_LENGTH * pow(MULTIPLY_FACTOR, i);

		it = index_0;
		/* Creating current line as a circular linked list */
		for (j = 0; j < req_sect - 1; j++) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->damage = 0;
			new->below = new->above = NULL;
			it->next = new;
			new->next = index_0;
			it = new;
		}

		/* Creating the above line if necessary and linking it */
		if (lines > 1 && i < lines - 1) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->damage = 0;
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
enum hdd_result hdd_print(struct hdd_sector *s)
{
	struct hdd_sector *it;
	struct hdd_sector *index_0;
	int line;

	if (s == NULL)
		return HDD_ERROR_INVALID_RESOURCE;

	index_0 = s;
	line = 0;
	while (FOREVER) {
		printf("LINE %d:\n", line);
		printf("\t%s(%d)", index_0->data, index_0->damage);

		/* Printing the current line */
		for (it = index_0->next; it != index_0; it = it->next)
			printf(" %s(%d)", it->data, it->damage);
		printf("\n");

		/* Moving to the above line, if it exists */
		if (index_0->above == NULL)
			break;
		else {
			index_0 = index_0->above;
			line++;
		}
	}

	return HDD_SUCCESS;
}

/* The drive head is always initialized on sector 0 on line 0 */
enum hdd_result hdd_head_init(struct hdd_head **h, struct hdd_sector *s)
{
	*h = (struct hdd_head *) malloc(sizeof(struct hdd_head));
	if (*h == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*h)->sect = s;

	(*h)->addr = (struct hdd_address *) malloc(sizeof(struct hdd_address));
	if ((*h)->addr == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*h)->addr->line = 0;
	(*h)->addr->index = 0;

	return HDD_SUCCESS;
}

/* Seeks an address by jumping one sector at a time. */
enum hdd_result hdd_seek(struct hdd_address *a, struct hdd_head *h)
{
	if (h == NULL || a == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	/* Checking if I'm on the right line shouldn't add any damage */
	if (a->line == h->addr->line && a->index == h->addr->index)
		return HDD_SUCCESS;

	/* Seeking the address */
	if (a->line > h->addr->line && h->addr->index == 0) {
		h->sect = h->sect->above;
		h->addr->line++;
	} else {
		h->sect = h->sect->next;
		if (h->sect->above != NULL)
			h->addr->index = 0;
		else
			h->addr->index++;
	}

	add_damage(h, CURSOR_DAMAGE);
	if (a->line == h->addr->line && a->index == h->addr->index)
		return HDD_SUCCESS;

	return HDD_SEEK_INCOMPLETE;
}

/* Reads data from the current sector */
enum hdd_result hdd_read_data(struct hdd_head *h, char *data)
{
	if (h == NULL || data == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	strncpy(data, h->sect->data, SECTOR_SIZE);
	add_damage(h, READ_DATA_DAMAGE + CURSOR_DAMAGE);

	return HDD_SUCCESS;
}

/* Writes data from the current sector */
enum hdd_result hdd_write_data(struct hdd_head *h, char *data)
{
	if (h == NULL || data == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	strncpy(h->sect->data, data, SECTOR_SIZE);
	add_damage(h, WRITE_DAMAGE + CURSOR_DAMAGE);

	return HDD_SUCCESS;
}

/* Read damage data from the current sector */
enum hdd_result hdd_read_damage(struct hdd_head *h, char *damage)
{
	if (h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	sprintf(damage, "%d", h->sect->damage);
	add_damage(h, READ_DAMAGE_DAMAGE + CURSOR_DAMAGE);

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

inline static void add_damage(struct hdd_head *h, int damage)
{
	h->sect->damage += damage;
}
