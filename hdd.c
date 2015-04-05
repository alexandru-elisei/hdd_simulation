#include <string.h>
#include <math.h>

#include "hdd.h"

#define READ_DATA_DAMAGE	(5)
#define WRITE_DAMAGE		(30)
#define READ_DAMAGE_DAMAGE	(2)
#define CURSOR_DAMAGE		(1)

#define FLOOR_2_DECIMALS(a, b)	(floor((float)(a)/(b) * 100) / 100)

/* Because I can */
inline static int power(int base, int exp);
inline static void add_damage(struct hdd_head *h, int damage);

/* Generates the hard drive */
enum hdd_result hdd_init(struct hdd_sector **s, int lines)
{
	struct hdd_sector *it;		/* Iterator for the list */
	struct hdd_sector *index_0;	/* Index 0 of current line */
	struct hdd_sector *new;		/* The new sector at each step */
	int i, j;
       	int req_sect;			/* Required sectors per line */

	if (lines <= 0)
		return HDD_ERROR_INVALID_PARAMETER;

	*s = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
	if (*s == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*s)->below = (*s)->above = NULL;
	(*s)->next = *s;
	strncpy((*s)->data, DEFAULT_VALUE, SECTOR_SIZE);
	(*s)->damage = 0;
	(*s)->index = 0;

	index_0 = *s;
	for (i = 1; i <= lines; i++) {
		req_sect = INITIAL_LINE_LENGTH * power(MULTIPLY_FACTOR, i - 1);

		it = index_0;
		/* Creating current line as a circular linked list */
		for (j = 1; j < req_sect; j++) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->damage = 0;
			new->index = j;
			new->below = new->above = NULL;
			new->next = index_0;
			it->next = new;
			it = new;
		}

		/* Creating the line above it if necessary and linking it */
		if (lines > 1 && i < lines) {
			new = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));
			strncpy(new->data, DEFAULT_VALUE, SECTOR_SIZE);
			new->damage = 0;
			new->index = 0;
			new->next = new;
			new->above = NULL;
			new->below = index_0;
			index_0->above = new;
			index_0 = new;
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
	(*h)->line = 0;

	return HDD_SUCCESS;
}

/* Seeks an address by jumping one sector at a time. */
enum hdd_result hdd_seek(struct hdd_address *a, struct hdd_head *h)
{
	if (h == NULL || a == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	/* Checking if I'm on the right line, not adding any damage */
	if (a->line == h->line && a->index == h->sect->index)
		return HDD_SUCCESS;

	/* 
	 * Seeking the address, jumping on the above/below line if necessary
	 * and possible
	 */
	if (h->sect->index == 0) {
		if (a->line > h->line) {
			h->sect = h->sect->above;
			h->line++;
		} else if (a->line < h->line) {
			h->sect = h->sect->below;
			h->line--;
		} else {
			h->sect = h->sect->next;
		}
	} else {
		h->sect = h->sect->next;
	}

	add_damage(h, CURSOR_DAMAGE);

	return HDD_SEEK_INCOMPLETE;
}

/* The drive head idles at a location */
enum hdd_result hdd_idle(struct hdd_head *h)
{
	if (h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	add_damage(h, CURSOR_DAMAGE);

	return HDD_SUCCESS;
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

/* Writes data in the current sector */
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
	if (h == NULL || damage == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	sprintf(damage, "%d", h->sect->damage);
	add_damage(h, READ_DAMAGE_DAMAGE + CURSOR_DAMAGE);

	return HDD_SUCCESS;
}

/* Calculate average damage across the hard drive */
enum hdd_result hdd_print_damage(struct hdd_sector *h, FILE *out)
{
	struct hdd_sector *it;
	struct hdd_sector *index_0;
	int line_num, sect_num;
	/* Borders for the four areas of the hard drive */
	int first_quarter, middle, third_quarter, end;
	/* Total damage for the four areas */
	int fq_damage, m_damage, tq_damage, e_damage;
	int sectors;

	if (h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	index_0 = h;
	line_num = sect_num = sectors = 0;
	fq_damage = m_damage = tq_damage = e_damage = 0;
	while (FOREVER) {
		/* Calculating the borders for the four areas */
		end = INITIAL_LINE_LENGTH * power(MULTIPLY_FACTOR, line_num);
		first_quarter = end / 4;
		middle = first_quarter * 2;
		third_quarter = 3 * first_quarter;

		sectors += end;

		fq_damage += index_0->damage;
		sect_num = 1;
		for (it = index_0->next; it != index_0; it = it->next) {
			if (sect_num < first_quarter)
				fq_damage += it->damage;
			else if (sect_num < middle)
				m_damage += it->damage;
			else if (sect_num < third_quarter)
				tq_damage += it->damage;
			else
				e_damage += it->damage;
			sect_num++;
		}

		/* End of hard drive */
		if(index_0->above == NULL)
			break;
		else {
			index_0 = index_0->above;
			line_num++;
		}
	}

	sectors /= 4;

	/* Average damage, rounded down to two decimals */
	fprintf(out, "%.2f", FLOOR_2_DECIMALS(fq_damage, sectors));
	fprintf(out, " %.2f", FLOOR_2_DECIMALS(m_damage, sectors));
	fprintf(out, " %.2f", FLOOR_2_DECIMALS(tq_damage, sectors));
	fprintf(out, " %.2f\n", FLOOR_2_DECIMALS(e_damage, sectors));

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

/* Frees allocated drive memory */
enum hdd_result hdd_destroy(struct hdd_sector **s)
{
	struct hdd_sector *aux;
	struct hdd_sector *it;
	struct hdd_sector *index_0;

	if (*s == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	index_0 = *s;
	while (FOREVER) {
		it = index_0->next;
		/* Freeing everything except the sector with index_0 */
		while (it != index_0) {
			aux = it;
			it = it->next;
			free(aux);
		}

		/* Reached index_0 on the outermost line */
		if (it->above == NULL) {
			free(it);
			break;
		/* I'm at index_0 */
		} else {
			aux = index_0;
			index_0 = index_0->above;
			free(aux);
		}
	}
	*s = NULL;

	return HDD_SUCCESS;
}

/* Frees allocated drive head memory */
enum hdd_result hdd_destroy_head(struct hdd_head **h)
{
	if (*h == NULL)
		return HDD_ERROR_INVALID_PARAMETER;

	free(*h);
	*h = NULL;

	return HDD_SUCCESS;
}

/* Base to the power of exp */
inline static int power(int base, int exp)
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
