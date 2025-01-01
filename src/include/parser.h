#pragma once
#ifndef _PARSER_H_
#define _PARSER_H_

#include <ast.h>
#include <lexer.h>

typedef struct parsing_error {
  char *msg;
  char bad_char;
} parsing_error_t;

parsing_error_t *parse(lexer_t *l, ast_t **ast);

#endif
