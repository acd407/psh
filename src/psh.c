#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  lexer_t *lexer = create_lexer();

  lex(lexer, "echo hello > > file.txt");

  ast_t *ast;
  parsing_error_t *err = parse(lexer, &ast);

  if (err != NULL) {
    fprintf(stderr, "psh: %s\n", err->msg);
    free(err);
    ast_destroy_node(ast);
    destroy_lexer(lexer);
    return 1;
  }

  ast_print(ast, 0);

  ast_destroy_node(ast);
  destroy_lexer(lexer);

  return 0;
}
