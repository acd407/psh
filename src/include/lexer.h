#pragma once
#ifndef _LEXER_H_
#define _LEXER_H_

enum token_type {
  TOKEN_ARGUMENT,
  TOKEN_PIPE,
  TOKEN_AMPERSAND,
  TOKEN_GREATER,
  TOKEN_LESSER,
  TOKEN_SEMICOLON,
};

enum state {
  IN_GENERAL,
  IN_QUOTE,
  IN_DQUOTE,
  IN_ESCAPESEQUENCE,
};

typedef struct token {
  char *lexeme;
  enum token_type type;
  struct token *next;
} token_t;

typedef struct lexical_error {
  char *msg;
  int character;
} lexical_error_t;

typedef struct lexer {
  token_t *root;
  lexical_error_t *error;
} lexer_t;

lexer_t *lexer_create(void);
token_t *lex(lexer_t *l, char *input);
void lexer_destroy(lexer_t *l);
char *token_to_string(token_t *t);

#endif
