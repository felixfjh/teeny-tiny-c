#ifndef LEXER_H
#define LEXER_H


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
	int curpos;

} lexer_t;

typedef struct
{
	const char *value;
	enum kind_e kind;

} token_t;

void lex_abort(const char *msg, ...);

void next_char(lexer_t *lx);
void skip_whitespace(lexer_t *lx);
void skip_comment(lexer_t *lx);

enum kind_e check_keyword(const char *str);
const char *enum_to_str(enum kind_e kind);

char peek(lexer_t *lx);

token_t *get_token(lexer_t *lx);

#endif /* LEXER_H */
