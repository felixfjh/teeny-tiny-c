#ifndef LEXER_H
#define LEXER_H

#include "globals.h"

void lex_abort(state_t *state, const char *msg, ...);

void next_char(lexer_t *lx);
void skip_whitespace(lexer_t *lx);
void skip_comment(lexer_t *lx);

enum kind_e check_keyword(const char *str);

char peek(lexer_t *lx);

#endif /* LEXER_H */
