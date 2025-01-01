#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <psh.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

void lexical_error(lexical_error_t *e) {
  eprintf("%s: %s: at character %d\n", PROGRAM_NAME, e->msg, e->character);
}

void parsing_error(parsing_error_t *e) {
  eprintf("%s: %s: '%s'\n", PROGRAM_NAME, e->msg, e->bad_tok);
}

int main(void) {
  lexer_t *lexer = lexer_create();

  if (lex(lexer, "echo hello") == NULL) {
    lexical_error(lexer->error);
    lexer_destroy(lexer);
    return 1;
  }

  parser_t *parser = parser_create();

  if (parse(parser, lexer) == NULL) {
    parsing_error(parser->err);

    parser_destroy(parser);
    lexer_destroy(lexer);
    return 1;
  }

  ast_print(parser->ast, 0);

  parser_destroy(parser);
  lexer_destroy(lexer);

  return 0;
}
