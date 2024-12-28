#pragma once
#ifndef _LEXER_H_
#define _LEXER_H_

enum token_type {
  ARGUMENT,
  PIPE = '|',
  AMPERSAND = '&',
  QUOTE = '\'',
  DQUOTE = '\"',
  SEMICOLON = ';',
  ESCAPE_SEQUENCE = '\\',
  NEWLINE = '\n',
  GREATER = '>',
  WHITESPACE = ' ',
  LESSER = '<',
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
  char *message;
  int character;
} lexical_error_t;

typedef struct lexer {
  token_t *root;
  lexical_error_t *error;
} lexer_t;

lexer_t *create_lexer(void);
void lex(lexer_t *l, char *input);
void destroy_lexer(lexer_t *l);
#endif
