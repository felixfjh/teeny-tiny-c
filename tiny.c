#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "lexer.h"

#define SIZE 1024

int main(int argc, char *argv[])
{
	char buffer[SIZE];
	printf("Teeny Tiny Compiler.\n");

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s path/to/teeny/file.\n", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "r");
	if (file == NULL)
	{
		fprintf(stderr, "Error: Couldn't open %s\n", argv[1]);
		return 1;
	}

	if (fread(buffer, 1, SIZE, file) == 0)
	{
		fprintf(stderr, "Error: Couldn't read %s\n", argv[1]);
		fclose(file);
		return 1;
	}
	
	fclose(file);

	lexer_t *lx = malloc(sizeof(lexer_t));
	if (lx == NULL)
	{
		fprintf(stderr, "Error: Couldn't allocate memory for lexer.\n");
		return 1;
	}

	set_t *sym = set_init();
	set_t *labels = set_init();
	set_t *gotoed = set_init();

	lx->src = buffer;
	lx->curpos = -1;

	next_char(lx);

	parser_t *ps = malloc(sizeof(parser_t));
	if (ps == NULL)
	{
		fprintf(stderr, "Error: Couldn't allocate memory for parser.\n");
		free_set(sym);
		free_set(labels);
		free_set(gotoed);
		free(lx);
		return 1;
	}

	state_t *state = malloc(sizeof(state_t));
	if (state == NULL)
	{
		fprintf(stderr, "Error Couldn't allocate memory for state.\n");
		free_set(sym);
		free_set(labels);
		free_set(gotoed);
		free(lx);
		free(ps);
		return 1;
	}

	state->lexer = lx;
	state->parser = ps;
	
	state->symbols = sym;
	state->labels = labels;
	state->gotoed = gotoed;

	ps->curtoken = get_token(state);
	ps->peektoken = get_token(state);

	program(state);

	return 0;
}
