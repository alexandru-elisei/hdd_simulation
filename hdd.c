#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	16
#define MULTIPLY_FACTOR		2
#define DEFAULT_VALUE		"0000"

#undef pow
inline static int pow(int base, int exp);

enum hdd_result hdd_init(struct hdd_sector **s, int lines)
{
#if 0
	struct hdd_sector *it;		/* iterator for the list */
	struct hdd_sector *index_0;	/* index 0 of current line */
	int i;
       	int sect_num; 			/* current sector number */
       	int line_num;			/* current line number */
       	int req_sect;			/* required sectors per line */

	if (lines <= 0)
		return HDD_ERROR_INVALID_PARAMETER;

	*s = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));

	if (*s == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*s)->below = (*s)->above = NULL;
	(*s)->next = *s;
	strcpy((*s)->data, DEFAULT_VALUE);
	it = index_0 = *s;

	req_sect = 16;
	for (line_num = 0; line_num < lines; line_num++) {
		req_sect = INITIAL_LINE_LENGTH * 
#endif
	
	printf("%d\n", pow(2, 3));

	return HDD_ERROR_MEMORY_ALLOC;

}

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
