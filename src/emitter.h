#ifndef EMITTER_H
#define EMITTER_H

#include "globals.h"

void emit(emitter_t *em, const char *code);
void emit_line(emitter_t *em, const char *code);

void header_line(emitter_t *em, const char *code);

void write_file(emitter_t *em);

#endif /* EMITTER_H */
