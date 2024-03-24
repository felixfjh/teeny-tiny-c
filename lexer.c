#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

void lex_abort(state_t *state, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, "Lexing error. ", NULL);
	vfprintf(stderr, msg, args);
	va_end(args);

	parser_t *ps = state->parser;
	lexer_t *lx = state->lexer;

	if (state != NULL)
	{
		if (ps->curtoken != NULL)
		{
			if (ps->curtoken->value != NULL &&
				is_keyword(ps->curtoken->kind) ||
			   	is_nis(ps->curtoken->kind))
			{
				free(ps->curtoken->value);
				printf("ps->curtoken->value freed!\n");
			}
			free(ps->curtoken);
			printf("ps->curtoken freed!\n");
		}
		
		/* ps->curtoken pointed to peektoken so don't have to */
		/* include this part */
		/* if (ps->peektoken != NULL) */
		/* { */
		/* 	if (ps->peektoken->value != NULL && */ 
		/* 		is_keyword(ps->peektoken->kind) || */
		/* 		is_nis(ps->peektoken->kind)) */
		/* 	{ */
		/* 		printf("peek value isn't NULL\n"); */
		/* 		free(ps->peektoken->value); */
		/* 		printf("ps->peektoken->value freed!\n"); */
		/* 	} */
		/* 	free(ps->peektoken); */
		/* 	printf("ps->peektoken freed!\n"); */
		/* } */

		if (ps != NULL)
		{
			free(ps);
			printf("ps freed!\n");
		}

		if (lx != NULL)
		{
			free(lx);
			printf("lx freed!\n");
		}

		free(state);
		printf("state freed!\n");
	}
	exit(1);
}

void next_char(lexer_t *lx)
{
	lx->curpos++;

	if (lx->curpos >= strlen(lx->src))
	{
		lx->curchar = '\0';
	}

	else
	{
		lx->curchar = lx->src[lx->curpos];
	}
}

void skip_whitespace(lexer_t *lx)
{
	while (lx->curchar == ' ' || lx->curchar == '\t' || lx->curchar == '\r')
	{
		next_char(lx);
	}
}

void skip_comment(lexer_t *lx)
{
	if (lx->curchar == '#')
	{
		while (lx->curchar != '\n')
		{
			next_char(lx);
		}
	}
}

enum kind_e check_keyword(const char *str)
{

	if (strncmp(str, "LABEL", 5) == 0)			{	return LABEL;		}
	else if (strncmp(str, "GOTO", 4) == 0)		{	return GOTO;		}
	else if (strncmp(str, "PRINT", 5) == 0)		{	return PRINT;		}
	else if (strncmp(str, "INPUT", 5) == 0)		{	return INPUT;		}
	else if (strncmp(str, "LET", 3) == 0)		{	return LET;			}
	else if (strncmp(str, "IF", 2) == 0)		{	return IF;			}
	else if (strncmp(str, "THEN", 4) == 0)		{	return THEN;		}
	else if (strncmp(str, "ENDIF", 5) == 0)		{	return ENDIF;		}
	else if (strncmp(str, "WHILE", 5) == 0)		{	return WHILE;		}
	else if (strncmp(str, "REPEAT", 6) == 0)    {	return REPEAT;		}
	else if (strncmp(str, "ENDWHILE", 8) == 0)  {	return ENDWHILE;	}

	return IDENT;
}

char peek(lexer_t *lx)
{
	if (lx->curpos + 1 >= strlen(lx->src))
	{
		return '\0';
	}
	else
	{
		return lx->src[lx->curpos +  1];
	}
}

token_t *get_token(state_t *state)
{
	token_t *tk = malloc(sizeof(token_t));
	lexer_t *lx = state->lexer;
	if (tk == NULL)
	{
		lex_abort(state, "Couldn't allocate memory for token.\n");
	}
	tk->value = NULL;

	skip_whitespace(lx);
	skip_comment(lx);

	switch (lx->curchar)
	{
		case '+':
			tk->value = "+";
			tk->kind = PLUS;
			break;
		case '-':
			tk->value = "-";
			tk->kind = MINUS;
			break;
		case '*':
			tk->value = "*";
			tk->kind = ASTK;
			break;
		case '/':
			tk->value = "/";
			tk->kind = SLASH;
			break;
		case '\n':
			tk->value = "\n";
			tk->kind = NEWLINE;
			break;
		case '=':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk->value = "==";
				tk->kind = EQEQ;
			}
			else
			{
				tk->value = "=";
				tk->kind = EQ;
			}
			break;
		case '>':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk->value = ">=";
				tk->kind = GTEQ;
			}
			else
			{
				tk->value = ">";
				tk->kind = GT;
			}
			break;
		case '<':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk->value = "<=";
				tk->kind = LTEQ;
			}
			else
			{
				tk->value = "<";
				tk->kind = LT;
			}
			break;
		case '!':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk->value = "!=";
				tk->kind = NOTEQ;
			}
			else
			{
				lex_abort(state, "Expected !=, not !%c\n", peek(lx));
			}
			break;
		case '\"':
			next_char(lx);
			int startpos = lx->curpos;

			while (lx->curchar != '\"')
			{
				if (lx->curchar == '\r' || lx->curchar == '\n' ||
					lx->curchar == '\t' || lx->curchar == '\\' ||
					lx->curchar == '%')
				{
					lex_abort(state, "Illegal character in string.\n");
				}

				next_char(lx);
			}

			int len = lx->curpos - startpos;
			char *str = malloc(len + 1); // +1 null terminator
			if (str == NULL)
			{
				lex_abort(state, "Couldn't allocate memory for string.\n");
			}
			strncpy(str, lx->src + startpos, len); //lx->src + startpos to go to string
			str[len] = '\0';
			tk->value = str;
			tk->kind = STRING;
			break;
		case '\0':
			tk->value = "\0";
			tk->kind = ENDF;
			break;
		default:
			if (isalpha(lx->curchar))
			{
				int start = lx->curpos;
				while (isalnum(lx->curchar))
				{
					next_char(lx);
				}

				int len = lx->curpos - start;
				char *str = malloc(len + 1);
				if (str == NULL)
				{
					lex_abort(state, "Couldn't allocate memory for string.\n");
				}
				strncpy(str, lx->src + start, len);

				int keyword = check_keyword(str);

				tk->value = str;
				tk->kind = keyword;

				return tk;
		 }

		 if (isdigit(lx->curchar))
		 {
			 int start = lx->curpos;
			 while (isdigit(lx->curchar))
			 {
				 next_char(lx);
			 }

			 if (lx->curchar == '.')
			 {
				 if (!isdigit(peek(lx)))
				 {
					 lex_abort(state, "Illegal character in number.\n");
				 }
				 next_char(lx);
				 while (isdigit(lx->curchar))
				 {
					 next_char(lx);
				 }
			 }

			 int len = lx->curpos - start;
			 char *str = malloc(len + 1);

			 if (str == NULL)
			 {
				 lex_abort(state, "Couldn't allocate memory for string.\n");
			 }

			 strncpy(str, lx->src + start, len);
			 str[len] = '\0';
			 tk->value = str;
			 tk->kind = NUMBER;
			 return tk;
		 }

		 break;
	}

	if (tk->value == NULL)
	{
		lex_abort(state, "Unknown Token: %c value: %i\n", lx->curchar, lx->curchar);
	}

	next_char(lx);
	return tk;
}
