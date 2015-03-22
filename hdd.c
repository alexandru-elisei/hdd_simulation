#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	16
#define MULTIPLY_FACTOR		2
#define DEFAULT_VALUE		"0000"

enum hdd_result hdd_init(struct hdd_sector **s, uint8_t lines)
{
#if 0
	struct hdd_sector *it;		/* iterator for the list */
	struct hdd_sector *index_0;		/* index 0 of current line */
	unsigned int i;
       	unsigned int sect_num; 		/* current sector number */
       	uint8_t line_num;		/* current line number */
       	unsigned int req_sect;		/* required sectors per line */

	if (lines <= 0)
		return HDD_ERROR_INVALID_PARAMETER;

	*s = (struct hdd_sector *) malloc(sizeof(struct hdd_sector));

	if (*s == NULL)
		return HDD_ERROR_MEMORY_ALLOC;

	(*s)->below = (*s)->above = NULL;
	(*s)->next = *s;
	strcpy((*s)->data, DEFAULT_VALUE);
	it = index_0 = *s;

	for (line_num = 0; line_num < lines; line_num++) {}
#endif

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

