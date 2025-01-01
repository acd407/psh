#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <stdlib.h>

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

static ast_t *simple_command(void) {
  token_t *t = curtok;
  if (t->type != TOKEN_ARGUMENT) {
    return NULL;
  }
  ast_t *ast = ast_create_node();
  ast->type = AST_SIMPLE_COMMAND;
  ast->token = t;

  if (next_token() != NULL) {
    ast->right = simple_command();
  }

  return ast;
}

static inline ast_t *handle_redirection(enum astype type, ast_t *simple) {
  token_t *nt = peek();
  if (nt->type != TOKEN_ARGUMENT) {
    return NULL;
  }
  ast_t *ast = ast_create_node();
  ast->type = type;
  ast->right = simple;
  ast->token = next_token();
  return ast;
}

static ast_t *command(void) {
  ast_t *simple = simple_command();

  if (simple == NULL) {
    return NULL;
  }

  if (match_token(TOKEN_GREATER)) {
    // <simple command> '>' <filename>
    return handle_redirection(AST_REDIRECT_OUT, simple);
  }
  if (match_token(TOKEN_LESSER)) {
    // <simple command> '<' <filename>
    return handle_redirection(AST_REDIRECT_IN, simple);
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

  // <pipe> '&' <full command> | <pipe> '&'
  if (match_token(TOKEN_AMPERSAND)) {
    ast_t *ast = ast_create_node();
    ast->type = AST_BACKGROUND;
    ast->left = left;
    next_token();
    ast->right = full_command();
    return ast;
  }

  // <pipe> ';' <full command> | <pipe> ';'
  if (match_token(TOKEN_SEMICOLON)) {
    ast_t *ast = ast_create_node();
    ast->type = AST_SEQUENCE;
    ast->left = left;
    next_token();
    ast->right = full_command();
    return ast;
  }

  return left;
}

static parsing_error_t *parseing_error_create(token_t *t) {
  (void)t;
  parsing_error_t *err = malloc(sizeof(parsing_error_t));
  err->msg = "parse error";
  return err;
}

parsing_error_t *parse(lexer_t *l, ast_t **ast) {
  parsing_error_t *err = NULL;
  curtok = l->root;
  *ast = full_command();
  if (peek() != NULL) {
    err = parseing_error_create(curtok);
  }
  return err;
}
