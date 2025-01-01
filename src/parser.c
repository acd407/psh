#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <stdlib.h>
#include <utils.h>

/* Our tiny shell grammar :)
 *
 * <full command> ::= <pipe> |
 * <pipe> '&' |
 * <pipe> '&' <full command> |
 * <pipe> ';' |
 * <pipe> ';' <full command>
 *
 * <pipe> ::= <command> | <command> '|' <pipe>
 *
 * <command> ::=  <simple command> '>' <filename> |
 * <simple command> '<' <filename> |
 * <simple command>
 *
 * <simple command> ::= <program name> | <program name> <command tail>
 */

static token_t *curtok = NULL;

static token_t *next_token(void) {
  if (curtok->next != NULL) {
    curtok = curtok->next;
    return curtok;
  }
  return NULL;
}

static token_t *peek(void) { return curtok->next; }

static int match_token(enum token_type type) { return curtok->type == type; }

static int expect_token(enum token_type type) {
  token_t *t = peek();
  return t != NULL && t->type == type;
}

static ast_t *simple_command(void) {
  token_t *t = curtok;
  if (t->type != TOKEN_ARGUMENT) {
    return NULL;
  }
  ast_t *ast = ast_create_node();
  ast->type = AST_COMMAND_TAIL;
  ast->token = t;

  if (next_token() != NULL) {
    ast->right = simple_command();
  }

  return ast;
}

static inline ast_t *handle_redirection(enum astype type, ast_t *simple) {
  if (!expect_token(TOKEN_ARGUMENT)) {
    return NULL;
  }
  ast_t *ast = ast_create_node();
  ast->type = type;
  ast->right = simple;
  ast->token = next_token();
  next_token();
  return ast;
}

static ast_t *command(void) {
  ast_t *simple = simple_command();

  if (simple == NULL) {
    return NULL;
  }

  if (match_token(TOKEN_GREATER)) {
    // <simple command> '>' <filename>
    ast_t *ast = handle_redirection(AST_REDIRECT_OUT, simple);
    if (ast == NULL) {
      ast_destroy_node(simple);
    }
    return ast;
  }
  if (match_token(TOKEN_LESSER)) {
    // <simple command> '<' <filename>
    ast_t *ast = handle_redirection(AST_REDIRECT_IN, simple);
    if (ast == NULL) {
      ast_destroy_node(simple);
    }
    return ast;
  }

  // <simple command>
  return simple;
}

static ast_t *pipe(void) {
  ast_t *left = command();

  if (left == NULL) {
    return NULL;
  }

  // <command> '|' <pipe>
  if (match_token(TOKEN_PIPE)) {
    if (!expect_token(TOKEN_ARGUMENT)) {
      ast_destroy_node(left);
      return NULL;
    }
    ast_t *ast = ast_create_node();
    ast->type = AST_PIPE;
    ast->left = left;
    next_token();
    ast->right = pipe();
    return ast;
  }

  // <command>
  return left;
}

static ast_t *full_command(void) {
  ast_t *left = pipe();

  if (left == NULL) {
    return NULL;
  }

  // <pipe> '&' <full command> | <pipe> '&'
  if (match_token(TOKEN_AMPERSAND)) {
    if (!expect_token(TOKEN_ARGUMENT) && peek() != NULL) {
      ast_destroy_node(left);
      return NULL;
    }
    ast_t *ast = ast_create_node();
    ast->type = AST_BACKGROUND;
    ast->left = left;
    next_token();
    ast->right = full_command();
    return ast;
  }

  // <pipe> ';' <full command> | <pipe> ';'
  if (match_token(TOKEN_SEMICOLON)) {
    if (!expect_token(TOKEN_ARGUMENT) && peek() != NULL) {
      ast_destroy_node(left);
      return NULL;
    }
    ast_t *ast = ast_create_node();
    ast->type = AST_SEQUENCE;
    ast->left = left;
    next_token();
    ast->right = full_command();
    return ast;
  }

  return left;
}

static void add_error(parser_t *p, token_t *t) {
  if (p->err != NULL) {
    free(p->err);
  }
  parsing_error_t *err = xmalloc(sizeof(parsing_error_t));
  err->msg = "parsing error: bad token";
  err->bad_tok = token_to_string(t);
  p->err = err;
}

parser_t *parser_create(void) {
  parser_t *p = xmalloc(sizeof(parser_t));
  p->ast = NULL;
  p->err = NULL;
  return p;
}

ast_t *parse(parser_t *parser, lexer_t *l) {
  curtok = l->root;
  parser->ast = full_command();
  if (peek() != NULL) {
    add_error(parser, peek());
    curtok = NULL;
    return NULL;
  }
  curtok = NULL;
  return parser->ast;
}

void parser_destroy(parser_t *p) {
  ast_destroy_node(p->ast);
  if (p->err) {
    free(p->err);
  }
  p->err = NULL;
  free(p);
  p = NULL;
}
