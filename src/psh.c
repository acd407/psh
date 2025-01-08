#include <eval.h>
#include <lexer.h>
#include <parser.h>
#include <psh.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

#include <readline/history.h>
#include <readline/readline.h>

static void lexical_error(lexical_error_t *e) {
  if (e == NULL) {
    return;
  }
  eprintf("%s: %s: at character %d\n", PROGRAM_NAME, e->msg, e->character);
}

static void parsing_error(parsing_error_t *e) {
  if (e == NULL) {
    return;
  }
  eprintf("%s: %s: '%s'\n", PROGRAM_NAME, e->msg, e->bad_tok);
}

static void trap_signals(void) {
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
}

static void do_input(char *input) {
  lexer_t *lexer = lexer_create();
  if (lex(lexer, input) == NULL) {
    lexical_error(lexer->error);
    lexer_destroy(lexer);
    return;
  }

  parser_t *parser = parser_create();

  if (parse(parser, lexer) == NULL) {
    parsing_error(parser->err);

    parser_destroy(parser);
    lexer_destroy(lexer);
    return;
  }

  evaluate(parser);

  parser_destroy(parser);
  lexer_destroy(lexer);
}

int main(void) {
  trap_signals();

  while (1) {
    char *input = readline(PROMPT);

    if (feof(stdin) || input == NULL) {
      exit(0);
    }

    add_history(input);

    do_input(input);

    free(input);
  }
  return 0;
}
