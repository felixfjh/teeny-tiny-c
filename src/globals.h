#ifndef GLOBALS_H
#define GLOBALS_H

#include "utils.h"

enum kind_e
{
	ENDF = -1,
	NEWLINE = 0,
	NUMBER = 1,
	IDENT = 2,
	STRING = 3,

	//string
	LABEL = 101,
	GOTO = 102,
	PRINT = 103,
	INPUT = 104,
	LET = 105,
	IF = 106,
	THEN = 107,
	ENDIF = 108,
	WHILE = 109,
	REPEAT = 110,
	ENDWHILE = 111,

	//operators
	EQ = 201,
	PLUS = 202,
	MINUS = 203,
	ASTK = 204,
	SLASH = 205,
	EQEQ = 206,
	NOTEQ = 207,
	LT = 208,
	LTEQ = 209,
	GT = 210,
	GTEQ = 211,

};

typedef struct
{
	const char *src;
	int curchar;
	size_t curpos;

} lexer_t;

typedef struct
{
	char *value;
	enum kind_e kind;

} token_t;

typedef struct
{
	token_t *curtoken;
	token_t *peektoken;

} parser_t;

typedef struct
{
	const char *path;
	char *header;
	char *code;

} emitter_t;

typedef struct
{
	lexer_t *lexer;
	parser_t *parser;
	emitter_t *emitter;

	set_t *symbols;
	set_t *labels;
	set_t *gotoed;

} state_t;

const char *enum_to_str(enum kind_e kind);

int is_keyword(enum kind_e kind);
int is_nis(enum kind_e kind);
int is_comparison(enum kind_e kind);

token_t *get_token(state_t *state);

#endif /* GLOBALS_H */
