#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

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
		return 1;
	}

	lexer_t lx;
	lx.src = buffer;
	lx.curpos = -1;

	next_char(&lx); // start at 0
	token_t *token = get_token(&lx);

	while (token->kind != ENDF)
	{
		/* printf("TokenValue.%s\n", token->value); */
		printf("TokenType.%s\n", enum_to_str(token->kind));
		if (token->kind >= 101 && token->kind <= 111 || token->kind == 1
				|| token->kind == 2 || token->kind == 3)
		{
			free(token->value);
		}
		free(token);
		token = get_token(&lx);
	}
	free(token); // free the ENDF token
	return 0;
}
