#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

typedef struct
{
	char **members;
	size_t length;

} set_t;

set_t *set_init();

void insert(set_t *set, char *str);
void free_set(set_t *set);

#endif /* UTILS_H */
