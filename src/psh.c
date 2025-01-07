#include <eval.h>
#include <lexer.h>
#include <parser.h>
#include <psh.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

#include <readline/history.h>
#include <readline/readline.h>

static void lexical_error(lexical_error_t *e) {
  eprintf("%s: %s: at character %d\n", PROGRAM_NAME, e->msg, e->character);
}

static void parsing_error(parsing_error_t *e) {
  eprintf("%s: %s: '%s'\n", PROGRAM_NAME, e->msg, e->bad_tok);
}

static void do_input(char *input) {
  lexer_t *lexer = lexer_create();
  if (lex(lexer, input) == NULL) {
    lexical_error(lexer->error);
    lexer_destroy(lexer);
  }

  parser_t *parser = parser_create();

  if (parse(parser, lexer) == NULL) {
    parsing_error(parser->err);

    parser_destroy(parser);
    lexer_destroy(lexer);
  }

  evaluate(parser);

  parser_destroy(parser);
  lexer_destroy(lexer);
}

int main(void) {

  while (1) {
    char *input = readline(PROMPT);

    add_history(input);

    do_input(input);

    free(input);
  }
  return 0;
}
