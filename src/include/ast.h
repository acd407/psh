#pragma once
#ifndef _AST_H_
#define _AST_H_

#include <lexer.h>

enum astype {
  AST_COMMAND_TAIL,
  AST_BACKGROUND,
  AST_SEQUENCE,
  AST_REDIRECT_IN,
  AST_PIPE,
  AST_REDIRECT_OUT,
};

typedef struct astree {
  enum astype type;
  token_t *token;
  struct astree *left;
  struct astree *right;
} ast_t;

ast_t *ast_create_node(void);
void ast_destroy_node(ast_t *node);
void ast_print(ast_t *node, int level);

#endif
