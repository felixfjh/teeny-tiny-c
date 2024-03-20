#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "praser.h"

void praser_abort(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, "Prasing Error: ", NULL);
	vfprintf(stderr, msg, args);
	va_end(args);
}

int check_token(praser_t *ps, enum kind_e kind)
{
	if (ps->curtoken->kind == kind)
	{
		return 1;
	}

	return 0;
}

int check_peek(praser_t *ps, enum kind_e kind)
{
	if (ps->peektoken->kind == kind)
	{
		return 1;
	}
	return 0;
}

void match(praser_t *ps, enum kind_e kind)
{
	if (!check_token(ps, kind))
	{
		praser_abort("Expected %s, not %s\n", enum_to_str(kind),
			enum_to_str(ps->curtoken->kind));
		if (kind >= 101 && kind <= 111 || kind == 1 || kind == 2 || kind == 3)
		{
			free(ps->curtoken->value);					
		}

		free(ps->curtoken);
		free(ps->peektoken);
		free(ps->lexer);
		free(ps);
		exit(1);
	}

	if (kind >= 101 && kind <= 111 || kind == 1 || kind == 2 || kind == 3)
	{
		free(ps->curtoken->value);
	}

	free(ps->curtoken);
	next_token(ps);
}

void next_token(praser_t *ps)
{
	ps->curtoken = ps->peektoken;
	ps->peektoken = get_token(ps->lexer);
}

void program(praser_t *ps)
{
	printf("PROGRAM\n");
	while (!check_token(ps, ENDF))
	{
		statement(ps);
	}

	free(ps->curtoken);
	free(ps->peektoken);
}

void statement(praser_t *ps)
{
	/* "PRINT" (expression | string) */
	if (check_token(ps, PRINT))
	{
		printf("STATEMENT-PRINT\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(ps);
		if(check_token(ps, STRING))
		{
			free(ps->curtoken->value);
			free(ps->curtoken);
			next_token(ps);
		}
	}

	/* "IF" comparison "THEN" {statement} "ENDIF" */
	else if (check_token(ps, IF))
	{
		printf("STATEMENT-IF\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(ps);
		match(ps, THEN);
		newline(ps);

		while (!check_token(ps, ENDIF))
		{
			statement(ps);
		}

		match(ps, ENDIF);
	}

	else if (check_token(ps, WHILE))
	{
		printf("STATEMENT-WHILE\n");
		free(ps->curtoken->value);
		free(ps->curtoken);
		
		next_token(ps);
		match(ps, REPEAT);
		newline(ps);

		while (!check_token(ps, ENDWHILE))
		{
			statement(ps);
		}

		match(ps, ENDWHILE);
	}

	newline(ps);
}

/* nl ::= '\n'+ */
void newline(praser_t *ps)
{
	printf("NEWLINE\n");
	match(ps, NEWLINE);
	while (check_token(ps, NEWLINE))
	{
		free(ps->curtoken);
		next_token(ps);
	}
}
