#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

set_t *set_init()
{
	set_t *new_set = malloc(sizeof(set_t));
	if (new_set == NULL)
	{
		fprintf(stderr, "Error: Couldn't allocate memory for a set.\n");
		exit(1);
	}

	new_set->members = NULL;
	new_set->length = 0;

	return new_set;
}

void insert(set_t *set, char *str)
{
	size_t string_len = strlen(str);
	for (size_t i = 0; i < set->length; i++)
	{
		if (strncmp(set->members[i], str, string_len) == 0)
		{
			return;
		}
	}

	char *cpy = strdup(str);
	if (cpy == NULL)
	{
		fprintf(stderr, "Error: Couldn't copy the string.\n");
		free_set(set);
		exit(1);
	}

	char **new_members = realloc(set->members,
		(set->length + 1) * sizeof(char*));

	if (new_members == NULL)
	{
		fprintf(stderr, "Error: Couldn't reallocate memory for a set.\n");
		free_set(set);
		exit(1);
	}
	set->members = new_members;
	set->members[set->length] = cpy;
	set->length++;
}

void free_set(set_t *set)
{
	if (set == NULL)
	{
		return;
	}

	for (size_t i = 0; i < set->length; i++)
	{
		if (set->members[i] != NULL)
		{
			free(set->members[i]);
		}
	}

	free(set->members);
	free(set);
}
