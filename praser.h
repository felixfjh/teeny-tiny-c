#ifndef PRASER_H
#define PRASER_H

#include "lexer.h"

typedef struct
{
	lexer_t *lexer;
	token_t *curtoken;
	token_t *peektoken;

} praser_t;

void praser_abort(const char *msg, ...);

int check_token(praser_t *ps, enum kind_e kind);
int check_peek(praser_t *ps, enum kind_e kind);

void match(praser_t *ps, enum kind_e kind);
void next_token(praser_t *ps);

void program(praser_t *ps);
void statement(praser_t *ps);
/* void expression(praser_t *ps); */
void newline(praser_t *ps);

#endif /* PRASER_H */
