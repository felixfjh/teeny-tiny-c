#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

void lex_abort(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, "Lexing error. ", NULL);
	vfprintf(stderr, msg, args);
	fprintf(stderr, "\n");
	va_end(args);
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
	else if (strncmp(str, "PRINT", 5) == 0) 	{	return PRINT;		}
	else if (strncmp(str, "INPUT", 5) == 0) 	{	return INPUT;		}
	else if (strncmp(str, "LET", 3) == 0)	  	{	return LET;			}
	else if (strncmp(str, "IF", 2) == 0)		{	return IF;			}
	else if (strncmp(str, "THEN", 4) == 0)	  	{	return THEN;		}
	else if (strncmp(str, "ENDIF", 5) == 0)	  	{	return ENDIF;		}
	else if (strncmp(str, "WHILE", 5) == 0) 	{	return WHILE;		}
	else if (strncmp(str, "REPEAT", 6) == 0)    {	return REPEAT;		}
	else if (strncmp(str, "ENDWHILE", 8) == 0)  {	return ENDWHILE;	}

	return IDENT;
}

const char *enum_to_str(enum kind_e kind)
{
	switch (kind)
	{
		case LABEL:		return "LABEL";		 break;
		case GOTO:	 	return "GOTO";		 break;
		case PRINT:	 	return "PRINT";		 break;
		case INPUT:		return "INPUT";		 break;
		case LET:	 	return "LET";		 break;
		case IF:	 	return "IF";		 break;
		case THEN:	 	return "THEN";		 break;
		case ENDIF:	 	return "ENDIF";		 break;
		case WHILE:	 	return "WHILE";		 break;
		case REPEAT: 	return "REPEAT";	 break;
		case ENDWHILE:	return "ENDWHILE";	 break;
	}

	return "IDENT"; 
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

token_t get_token(lexer_t *lx)
{
	token_t tk;
	tk.value = NULL;

	skip_whitespace(lx);
	skip_comment(lx);

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
			lex_abort("Couldn't allocate memory for string.\n");
		}
		strncpy(str, lx->src + start, len);
		str[len] = '\0';
		int keyword = check_keyword(str);

		tk.value = str;
		tk.kind = keyword;
		const char *keyword_str = enum_to_str(keyword);
		/* if (keyword_str == NULL) */
		/* { */
		/* 	lex_abort("Unkown Token: %c value %c\n", tk.value, tk.value); */
		/* } */

		printf("TokenType.%s\n", keyword_str);

		/* printf("str: %s\n", str); */
		free(str);
	}

	switch (lx->curchar)
	{
		case '+':
			tk.value = "+";
			tk.kind = PLUS;
			printf("TokenType.PLUS\n");
			break;
		case '-':
			tk.value = "-";
			tk.kind = MINUS;
			printf("TokenType.MINUS\n");
			break;
		case '*':
			tk.value = "*";
			tk.kind = ASTK;
			printf("TokenType.ASTK\n");
			break;
		case '/':
			tk.value = "/";
			tk.kind = SLASH;
			printf("TokenType.SLASH\n");
			break;
		case '\n':
			tk.value = "\n";
			tk.kind = NEWLINE;
			printf("TokenType.NEWLINE\n");
			break;
		case '=':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk.value = "==";
				tk.kind = EQEQ;
				printf("TokenType.EQEQ\n");
			}
			else
			{
				tk.value = "=";
				tk.kind = EQ;
				printf("TokenType.EQ\n");
			}
			break;
		case '>':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk.value = ">=";
				tk.kind = GTEQ;
				printf("TokenType.GTEQ\n");
			}
			else
			{
				tk.value = ">";
				tk.kind = GT;
				printf("TokenType.GT\n");
			}
			break;
		case '<':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk.value = "<=";
				tk.kind = LTEQ;
				printf("TokenType.LTEQ\n");
			}
			else
			{
				tk.value = "<";
				tk.kind = LT;
				printf("TokenType.LT\n");
			}
			break;
		case '!':
			if (peek(lx) == '=')
			{
				next_char(lx);
				tk.value = "!=";
				tk.kind = NOTEQ;
				printf("TokenType.NOTEQ\n");
			}
			else
			{
				lex_abort("Expected !=, not !%c\n", peek(lx));
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
					lex_abort("Illegal character in string.\n");
				}
				next_char(lx);
			}
			int len = lx->curpos - startpos;
			char *str = malloc(len + 1); // +1 null terminator
			if (str == NULL)
			{
				lex_abort("Couldn't allocate memory for string.\n");
			}
			strncpy(str, lx->src + startpos, len); //lx->src + startpos to go to string
			str[len] = '\0';
			tk.value = str;
			tk.kind = STRING;
			printf("TokenType.STRING\n");
			free(str);
			break;
		/* case '\0': */
		/* 	tk.value = "\0"; */
		/* 	tk.kind = ENDF; */
		/* 	printf("TokenType.ENDF\n"); */
		/* 	break; */
	}

	if (isdigit(lx->curchar))
	{
		int start = lx->curpos;
		while (isdigit(peek(lx)))
		{
			next_char(lx);
		}
		if (peek(lx) == '.')
		{
			//check after decimal
			next_char(lx);
			if (!isdigit(peek(lx)))
			{
				lex_abort("Illegeal character in number.\n");
			}
			while (isdigit(peek(lx)))
			{
				next_char(lx);
			}
		}
		int len = lx->curpos - start;
		char *str = malloc(len + 1);
		if (str == NULL)
		{
			lex_abort("Couldn't allocate memory for string.\n");
		}
		strncpy(str, lx->src + start, len);
		str[len] = '\0';
		tk.value = str;
		tk.kind = NUMBER;
		printf("TokenType.NUMBER\n");
		free(str);
	}

	if (lx->curchar == '\0')
	{
		tk.value = "\0";
		tk.kind = ENDF;
	}

	if (tk.value == NULL)
	{
		lex_abort("Unknown Token: %c value: %i", lx->curchar, lx->curchar);
	}

	next_char(lx);
	return tk;
}
