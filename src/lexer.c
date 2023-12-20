#include <lexer.h>
#include <stdlib.h>
#include <string.h>

static void createToken(token *t, char *input) {
  t->next = NULL;
  t->type = NULL_C;
  t->text = input;
  t->size = 0;
}

static int getCharType(char c) {
  switch (c) {
  case '&':
    return AMPERSAND;
    break;
  case '|':
    return PIPE;
    break;
  case ';':
    return SEMICOLON;
    break;
  case ' ':
  case '\t':
    return WHITESPACE;
    break;
  case '>':
    return GREATER;
    break;
  case '<':
    return LESSER;
    break;
  case '\'':
    return QUOTE;
    break;
  case '\"':
    return DQUOTE;
    break;
  case '\\':
    return ESCAPESEQUENCE;
    break;
  case '\n':
    return NEWLINE;
    break;
  default:
    return GENERAL;
    break;
  }
  return GENERAL;
}

int createLexer(char *input, lexer *l) {
  if (input[strlen(input) - 1] == '\n') {
    input[strlen(input) - 1] = '\0';
  }
  token *t = malloc(sizeof(token));
  createToken(t, input);
  l->root = t;
  int i = 0;
  char c = '\0';
  int state = IN_GENERAL;

  do {
    c = input[i];
    int ctype = getCharType(c);
    if (state == IN_GENERAL) {
      switch (ctype) {
      case GENERAL:
        t->size++;
        t->type = NORMAL;
        break;
      case QUOTE:
        t->type = NORMAL;
        state = IN_QUOTE;
        t->size++;
        break;
      case DQUOTE:
        t->type = NORMAL;
        state = IN_DQUOTE;
        t->size++;
        break;
      case WHITESPACE:
        if (t->size > 0) {
          t->text[t->size] = '\0';
          t->next = malloc(sizeof(token));
          createToken(t->next, input + i + 1);
          t = t->next;
        }
        break;
      case ESCAPESEQUENCE:
        t->type = NORMAL;
        t->size++;
        i++;
        break;
      case AMPERSAND:
      case SEMICOLON:
      case PIPE:
      case GREATER:
      case LESSER:
        t->size++;
        t->type = ctype;
      }
    } else if (state == IN_DQUOTE) {
      t->size++;
      if (ctype == IN_DQUOTE) {
        state = IN_GENERAL;
      }
    } else if (state == IN_QUOTE) {
      t->size++;
      if (ctype == IN_QUOTE) {
        state = IN_GENERAL;
      }
    }
    i++;
  } while (c != '\0');

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
}
