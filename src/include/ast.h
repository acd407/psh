#pragma once
#ifndef _AST_H_
#define _AST_H_
enum astype {
  AST_PIPE,
  AST_ARG,
  AST_CMD,
  AST_REDIN,
  AST_REDOUT,
  AST_BCKGRND,
  AST_SEQ,
};

typedef struct astree {
  enum astype type;
  char *data;
  struct astree *left;
  struct astree *right;
} astree;

astree *newAst(void);
void setData(astree *a, char *data);
void setChilds(astree *a, astree *l, astree *r);
void setNodeType(astree *a, enum astype nodetype);
#endif
