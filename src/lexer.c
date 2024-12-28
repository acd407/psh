#define _DEFAULT_SOURCE
#include <lexer.h>
#include <stdlib.h>
#include <string.h>

lexer_t *create_lexer(void) {
  lexer_t *l = malloc(sizeof(lexer_t));
  l->root = NULL;
  l->error = NULL;
  return l;
}

static token_t *add_token(token_t *ptok, char *lexeme, enum token_type type) {
  token_t *t = malloc(sizeof(token_t));
  if (lexeme) {
    t->lexeme = strdup(lexeme);
  } else {
    t->lexeme = NULL;
  }
  t->type = type;
  t->next = NULL;
  if (ptok) {
    ptok->next = t;
  }
  return t;
}

static enum token_type get_type(char ch) {
  switch (ch) {
  case '|':
    return PIPE;
    break;
  case '&':
    return AMPERSAND;
    break;
  case '<':
    return LESSER;
    break;
  case '>':
    return GREATER;
    break;
  default:
    return ARGUMENT;
    break;
  }
}

static int advace_string(char *from, char *to) {
  char type = *from;
  int i = 1;
  while (from[i] != '\0') {
    if (from[i] == type) {
      return i + 1;
    }
    to[i - 1] = from[i];
    i++;
  }

  return 0;
}

static void add_error(lexer_t *l, char *message, int character) {
  if (l->error) {
    free(l->error->message);
    free(l->error);
  }
  l->error = malloc(sizeof(lexical_error_t));
  l->error->message = message;
  l->error->character = character;
}

void lex(lexer_t *l, char *input) {
  char lexeme[strlen(input) + 1];
  token_t *t = NULL;
  int add = 0;

  int i = 0;
  int lexeme_i = 0;
  while (input[i] != '\0') {
    switch (input[i]) {
    case ' ':
    case '\t':
    case '\n':
      if (lexeme_i > 0) {
        lexeme[lexeme_i] = '\0';
        lexeme_i = 0;
        t = add_token(t, lexeme, ARGUMENT);
        if (!l->root) {
          l->root = t;
        }
      }
      break;
    case '&':
    case '>':
    case '<':
    case '|':
      lexeme[lexeme_i] = '\0';
      lexeme_i = 0;
      t = add_token(t, lexeme, ARGUMENT);
      if (!l->root) {
        l->root = t;
      }
      t = add_token(t, NULL, get_type(input[i]));
      break;
    case ';':
      lexeme[lexeme_i] = '\0';
      t = add_token(t, lexeme, ARGUMENT);
      break;
    case '\'':
    case '\"':
      add = advace_string(input + i, lexeme);
      if (add == 0) {
        add_error(l, "Unmatched quote", i);
        return;
      }
      lexeme_i += add - 2;
      i += add;
      break;
    case '\\':
      if (input[i + 1] == '\0') {
        add_error(l, "Escape sequence at end of input", i);
        return;
      }
      lexeme[lexeme_i++] = input[i + 1];
      i++;
      break;
    default:
      lexeme[lexeme_i++] = input[i];
      break;
    }
    i++;
  }
  if (lexeme_i > 0) {
    lexeme[lexeme_i] = '\0';
    add_token(t, lexeme, ARGUMENT);
  }
}

void destroy_lexer(lexer_t *l) {
  token_t *t = l->root;
  while (t) {
    token_t *next = t->next;
    free(t->lexeme);
    free(t);
    t = next;
  }
  if (l->error) {
    free(l->error->message);
    free(l->error);
  }
  free(l);
  l = NULL;
}
