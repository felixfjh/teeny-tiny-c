#include <stdlib.h>
#include <string.h>

#include "emitter.h"

void emit(emitter_t *em, const char *code)
{
	size_t old = strlen(em->code);
	size_t new = strlen(code);
	size_t total = old + new + 1;

	em->code = realloc(em->code, total);
	if (em->code == NULL)
	{
		fprintf(stderr, "Error: Couldn't reallocate memory for emitting code.\n");
		free(em);
		exit(1);
	}

	strncat(em->code, code, new);
	/* printf("code: %s\n", em->code); */
}

void emit_line(emitter_t *em, const char *code)
{
	size_t old = strlen(em->code);
	size_t new = strlen(code);
	size_t total = old + new + 2; /* 2 for newline and NULL terminator */

	em->code = realloc(em->code, total);
	if (em->code == NULL)
	{
		fprintf(stderr, "Error: Couldn't reallocate memory for emitting code.\n");
		free(em);
		exit(1);
	}

	strncat(em->code, code, new);
	strncat(em->code, "\n", 1);
	/* printf("line: %s", em->code); */

}

void header_line(emitter_t *em, const char *code)
{
	size_t old = strlen(em->header);
	size_t new = strlen(code);
	size_t total = old + new + 1;

	em->header = realloc(em->header, total);
	if (em->header == NULL)
	{
		fprintf(stderr, "Error: Couldn't reallocate memory emitting header.\n");
		if (em->code != NULL)
		{
			free(em->code);
		}
		free(em);
		exit(1);
	}
	strncat(em->header, code, new);
	/* printf("header: %s", em->header); */
}

void write_file(emitter_t *em)
{
	FILE *file = fopen(em->path, "w");
	if (file == NULL)
	{
		fprintf(stderr, "Error: Couldn't open file %s for writing.\n", em->path);
		free(em->code);
		free(em->header);
		free(em);
		exit(1);
	}

	if (em->header != NULL)
	{
		fputs(em->header, file);
	}

	if (em->code != NULL)
	{
		fputs(em->code, file);
	}

	fclose(file);
}
