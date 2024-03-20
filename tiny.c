#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "praser.h"

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
		return 1;
	}

	lexer_t *lx = malloc(sizeof(lexer_t));
	lx->src = buffer;
	lx->curpos = -1;

	next_char(lx); // start at 0

	praser_t *ps = malloc(sizeof(praser_t));
	ps->lexer = lx;
	ps->curtoken = get_token(lx);
	ps->peektoken = get_token(lx);

	program(ps);

	printf("Prasing Completed!\n");

	free(lx);
	free(ps);
	return 0;
}
