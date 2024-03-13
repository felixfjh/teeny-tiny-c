#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

int main(void)
{
	printf("Teeny Tiny Compiler.\n");
	lexer_t lx;
	lx.src = "IF+-123 foo*THEN/";
	lx.curpos = -1;

	next_char(&lx); // start at 0
	token_t token = get_token(&lx);

	while (token.kind != ENDF)
	{
		token = get_token(&lx);
	}

	return 0;
}
