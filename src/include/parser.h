#pragma once
#ifndef _PARSER_H_
#define _PARSER_H_

#include <ast.h>
#include <lexer.h>

typedef struct parsing_error {
    char *msg;
    char *bad_tok;
} parsing_error_t;

typedef struct parser {
    ast_t *ast;
    parsing_error_t *err;
} parser_t;

parser_t *parser_create (void);
ast_t *parse (parser_t *parser, lexer_t *l);
void parser_destroy (parser_t *p);

#endif
