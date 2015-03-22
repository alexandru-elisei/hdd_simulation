#include <string.h>

#include "hdd.h"

#define INITIAL_LINE_LENGTH	16
#define MULTIPLY_FACTOR		2
#define DEFAULT_VALUE		"AAAA"

enum hdd_result hdd_init(struct Sector **s, uint8_t lines)
{
#if 0
	
	struct Sector *it;		/* Iterator for the list */
	struct Sector *index_0;		/* Index 0 of current line */
	unsigned int i;
       	unsigned int sect_num; 		/* Current sector number */
       	unsigned int line_num;		/* Current line number */
       	unsigned int req_sect;		/* Required sectors per line */

	if (lines <= 0)
		return HDD_ERROR_INVALID_PARAMETER;

	*s = (struct Sector *) malloc(sizeof(struct Sector));

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

enum hdd_result hdd_head_init(struct Hddhead **h)
{
	*h = (struct Hddhead *) malloc(sizeof(struct Hddhead));
	(*h)->sect = (struct Sector *) malloc(sizeof(struct Sector));
	(*h)->line = 0;
	(*h)->index = 0;

	return HDD_SUCCESS;
}

