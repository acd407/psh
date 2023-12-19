#include <lexer.h>
#include <stdlib.h>
#include <string.h>

int createLexer(char *input, lexer *l) {
  token *t = malloc(sizeof(token));
  l->root = t;
  t->type = NULL_C;
  t->next = NULL;
  t->text = strtok(input, "\t\n\r ");
  while (t->text != NULL) {
    switch (t->text[0]) {
    case '|':
      t->type = PIPE;
      break;
    case '>':
      t->type = GREATER;
      break;
    case '<':
      t->type = LESSER;
      break;
    case '&':
      t->type = AMPERSAND;
      break;
    case ';':
      t->type = SEMICOLON;
      break;
    case '\'':
      t->type = QOUTE;
      break;
    case '\"':
      t->type = DQUOTE;
      break;
    default:
      t->type = GENERAL;
      break;
    }
    t->next = malloc(sizeof(token));
    if (t->next == NULL) {
      return -1;
    }
    t = t->next;
    t->type = NULL_C;
    t->next = NULL;
    t->text = strtok(NULL, "\t\n\r ");
  }

  return 0;
}

void destroyLexer(lexer *l) {
  token *t = l->root;
  token *tmp;
  while (t != NULL) {
    tmp = t->next;
    free(t);
    t = tmp;
  }
  free(l);
}
