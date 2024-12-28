#include <lexer.h>
#include <stdio.h>

int main(void) {
  lexer_t *lexer = create_lexer();

  lex(lexer, "echo 'Hello, World!' | grep \\'Hello\\'");

  for (token_t *t = lexer->root; t; t = t->next) {
    printf("Token: %s\n", t->lexeme);
    printf("Type: %d\n", t->type);
  }

  destroy_lexer(lexer);

  return 0;
}
