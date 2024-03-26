#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "parser.h"

void parser_abort(state_t *state, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, "Parsing Error: ", NULL);
	vfprintf(stderr, msg, args);
	va_end(args);

	parser_t *ps = state->parser;
	lexer_t *lx = state->lexer;

	if (state != NULL)
	{
		if (ps->curtoken != NULL)
		{
			if ((ps->curtoken->value != NULL &&
				is_keyword(ps->curtoken->kind)) ||
			   	is_nis(ps->curtoken->kind))
			{
				free(ps->curtoken->value);
				printf("ps->curtoken->value freed!\n");
			}
			free(ps->curtoken);
			printf("ps->curtoken freed!\n");
		}

		if (ps->peektoken != NULL)
		{
			if ((ps->peektoken->value != NULL && 
				is_keyword(ps->peektoken->kind)) ||
				is_nis(ps->peektoken->kind))
			{
				free(ps->peektoken->value);
				printf("ps->peektoken->value freed!\n");
			}
			free(ps->peektoken);
			printf("ps->peektoken freed!\n");
		}

		free_set(state->symbols);
		free_set(state->labels);
		free_set(state->gotoed);

		if (state->emitter->code != NULL)
		{
			free(state->emitter->code);
			printf("state->emitter->code freed!\n");
		}

		if (state->emitter->header != NULL)
		{
			free(state->emitter->header);
			printf("state->emitter->header freed!\n");
		}

		if (state->emitter != NULL)
		{
			free(state->emitter);
			printf("state->emitter freed!\n");
		}

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

int check_token(parser_t *ps, enum kind_e kind)
{
	if (ps->curtoken->kind == kind)
	{
		return 1;
	}

	return 0;
}

int check_peek(parser_t *ps, enum kind_e kind)
{
	if (ps->peektoken->kind == kind)
	{
		return 1;
	}
	return 0;
}

void match(state_t *state, enum kind_e kind)
{
	if (!check_token(state->parser, kind))
	{
		parser_abort(state, "Expected %s, not %s\n", enum_to_str(kind),
			enum_to_str(state->parser->curtoken->kind));
	}

	if (is_keyword(state->parser->curtoken->kind) ||
		is_nis(state->parser->curtoken->kind))
	{
		free(state->parser->curtoken->value);
	}

	free(state->parser->curtoken);
	next_token(state);
}

void next_token(state_t *state)
{
	state->parser->curtoken = state->parser->peektoken;
	state->parser->peektoken = get_token(state);
}

void program(state_t *state)
{
	printf("PROGRAM\n");

	header_line(state->emitter, "#include <stdio.h>\n");
	header_line(state->emitter, "int main(void) {\n");

	// skip newline at the beginning
	while (check_token(state->parser, NEWLINE))
	{
		free(state->parser->curtoken);
		next_token(state);
	}

	while (!check_token(state->parser, ENDF))
	{
		statement(state);
	}

	emit_line(state->emitter, "return 0;");
	emit_line(state->emitter, "}");
	write_file(state->emitter);

	check_goto(state);

	printf("Compiling Completed!\n");
	free(state->parser->curtoken);
	free(state->parser->peektoken);
	free_set(state->symbols);
	free_set(state->labels);
	free_set(state->gotoed);
	free(state->emitter->code);
	free(state->emitter->header);
	free(state->emitter);
	free(state->parser);
	free(state->lexer);
	free(state);
}

void statement(state_t *state)
{
	parser_t *ps = state->parser;
	/* "PRINT" (expression | string) */
	if (check_token(ps, PRINT))
	{
		printf("STATEMENT-PRINT\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);
		if(check_token(ps, STRING))
		{
			emit(state->emitter, "printf(\"");
			emit(state->emitter, ps->curtoken->value);
			emit_line(state->emitter, "\\n\");");
			free(ps->curtoken->value);
			free(ps->curtoken);
			next_token(state);
		}
		else
		{
			emit(state->emitter, "printf(\"%");
			emit(state->emitter, ".2f\\n\", (float)(");
			expression(state);
			emit_line(state->emitter, "));");
		}
	}

	/* "IF" comparison "THEN" {statement} "ENDIF" */
	else if (check_token(ps, IF))
	{
		printf("STATEMENT-IF\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);
		emit(state->emitter, "if(");

		comparison(state);
		match(state, THEN);
		newline(state);
		emit_line(state->emitter, "){");

		while (!check_token(ps, ENDIF))
		{
			statement(state);
		}

		match(state, ENDIF);
		emit_line(state->emitter, "}");
	}

	/* "WHILE" comparsion "REPEAT" {statement} "ENDWHILE" */
	else if (check_token(ps, WHILE))
	{
		printf("STATEMENT-WHILE\n");
		free(ps->curtoken->value);
		free(ps->curtoken);
		
		next_token(state);
		emit(state->emitter, "while(");

		comparison(state);
		match(state, REPEAT);
		newline(state);

		emit_line(state->emitter, "){");

		while (!check_token(ps, ENDWHILE))
		{
			statement(state);
		}

		match(state, ENDWHILE);
		emit_line(state->emitter, "}");
	}

	/* "LABEL" ident nl */
	else if (check_token(ps, LABEL))
	{
		printf("STATEMENT-LABEL\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);

		int found = 0;
		for (size_t i = 0; i < state->labels->length; i++)
		{
			if (strncmp(state->labels->members[i], ps->curtoken->value,
					strlen(ps->curtoken->value)) == 0)
			{
				found = 1;
				break;
			}
		}

		if (found)
		{
			parser_abort(state, "Label already exists: %s\n", ps->curtoken->value);
		}

		insert(state->labels, ps->curtoken->value);
		emit(state->emitter, ps->curtoken->value);
		emit_line(state->emitter, ":");
		match(state, IDENT);
	}

	/* "GOTO" ident nl */
	else if (check_token(ps, GOTO))
	{
		printf("STATEMENT-GOTO\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);
		insert(state->gotoed, ps->curtoken->value);

		emit(state->emitter, "goto ");
		emit(state->emitter, ps->curtoken->value);
		emit_line(state->emitter, ";");

		match(state, IDENT);
	}

	/* "LET" ident "=" expression */
	else if (check_token(ps, LET))
	{
		printf("STATEMENT-LET\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);

		int found = 0;
		for (size_t i = 0; i < state->symbols->length; i++)
		{
			if (strncmp(state->symbols->members[i], ps->curtoken->value,
				strlen(ps->curtoken->value)) == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
		{
			insert(state->symbols, ps->curtoken->value);
			header_line(state->emitter, "float ");
			header_line(state->emitter, ps->curtoken->value);
			header_line(state->emitter, ";\n");
		}

		emit(state->emitter, ps->curtoken->value);
		emit(state->emitter, " = ");

		match(state, IDENT);
		match(state, EQ);
		expression(state);

		emit_line(state->emitter, ";");
	}

	/* "INPUT" ident */
	else if (check_token(ps, INPUT))
	{
		printf("STATEMENT-INPUT\n");
		free(ps->curtoken->value);
		free(ps->curtoken);

		next_token(state);

		int found = 0;
		for (size_t i = 0; i < state->symbols->length; i++)
		{
			if (strncmp(state->symbols->members[i], ps->curtoken->value,
				strlen(ps->curtoken->value)) == 0)
			{
				found = 1;
				break;
			}
		}

		if (!found)
		{
			insert(state->symbols, ps->curtoken->value);
			header_line(state->emitter, "float ");
			header_line(state->emitter, ps->curtoken->value);
			header_line(state->emitter, ";\n");
		}
		/* if scanf returns 0, the input is invalid. */
		emit(state->emitter, "if (scanf(\"%f\", &");
		emit(state->emitter, ps->curtoken->value);
		emit(state->emitter, ") == 0) {\n");
		emit(state->emitter, ps->curtoken->value);
		emit_line(state->emitter, " = 0;");
		emit_line(state->emitter, "scanf(\"%*s\");");
		emit_line(state->emitter, "}");
		match(state, IDENT);
	}
	else
	{
		parser_abort(state, "Invalid statement at %s ( %s )\n",
			ps->curtoken->value, enum_to_str(ps->curtoken->kind));
	}

	newline(state);
}

/* nl ::= '\n'+ */
void newline(state_t *state)
{
	printf("NEWLINE\n");
	match(state, NEWLINE);
	while (check_token(state->parser, NEWLINE))
	{
		free(state->parser->curtoken);
		next_token(state);
	}
}

void check_goto(state_t *state)
{
	int found = 0;
	for (size_t i = 0; i < state->gotoed->length; i++)
	{
		for (size_t j = 0; j < state->labels->length; j++)
		{
			if (strncmp(state->gotoed->members[i], state->labels->members[j],
				strlen(state->labels->members[i])) == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
		{
			parser_abort(state, "Attempting to GOTO to undeclared label: %s\n",
				state->gotoed->members[i]);
		}
	}
}

/* comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=")) expression+ */
void comparison(state_t *state)
{
	printf("COMPARISON\n");
	expression(state);

	/* at least one comparison operator is required */
	if (is_comparison(state->parser->curtoken->kind))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken);
		next_token(state);
		expression(state);
	}
	else
	{
		parser_abort(state, "Expected comparison operator at: %s\n", 
			state->parser->curtoken->value);
	}

	while (is_comparison(state->parser->curtoken->kind))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken);
		next_token(state);
		expression(state);
	}
}

/* expression ::= term {( "-" | "+" )} term */
void expression(state_t *state)
{
	printf("EXPRESSION\n");
	term(state);

	while (check_token(state->parser, MINUS) ||
		  check_token(state->parser, PLUS))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken);
		next_token(state);
		term(state);
	}
}

/* term ::= unary {("/" | "*")} unary */
void term(state_t *state)
{
	printf("TERM\n");
	unary(state);

	while (check_token(state->parser, SLASH) ||
		  check_token(state->parser, ASTK))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken);
		next_token(state);
		unary(state);
	}
}

/* unary ::= ["+" | "-"] primary */
void unary(state_t *state)
{
	printf("UNARY\n");

	if (check_token(state->parser, PLUS) ||
	   check_token(state->parser, MINUS))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken);
		next_token(state);
	}
	primary(state);
}

/* primary ::= number | ident */
void primary(state_t *state)
{
	printf("PRIMARY ( %s )\n", state->parser->curtoken->value);

	if (check_token(state->parser, NUMBER))
	{
		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken->value);
		free(state->parser->curtoken);

		next_token(state);
	}
	else if (check_token(state->parser, IDENT))
	{
		int found = 0;
		for (size_t i = 0; i < state->symbols->length; i++)
		{
			if (strncmp(state->symbols->members[i], state->parser->curtoken->value,
				strlen(state->parser->curtoken->value)) == 0)
			{
				found = 1;
				break;
			}
		}

		if (!found)
		{
			parser_abort(state, "Referencing variable before assignment: %s\n",
				state->parser->curtoken->value);
		}

		emit(state->emitter, state->parser->curtoken->value);
		free(state->parser->curtoken->value);
		free(state->parser->curtoken);

		next_token(state);
	}
	else
	{
		parser_abort(state, "Unexpected token at %s\n",
			state->parser->curtoken->value);
	}
}
