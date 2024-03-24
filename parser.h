#ifndef PARSER_H
#define PARSER_H

#include "globals.h"

void parser_abort(state_t *state, const char *msg, ...);

int check_token(parser_t *ps, enum kind_e kind);
int check_peek(parser_t *ps, enum kind_e kind);

void match(state_t *state, enum kind_e kind);
void next_token(state_t *state);

void program(state_t *state);
void statement(state_t *state);
void newline(state_t *state);

void expression(state_t *state);
void comparison(state_t *state);
void term(state_t *state);
void unary(state_t *state);
void primary(state_t *state);

#endif /* PARSER_H */
