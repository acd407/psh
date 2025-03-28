#include <ast.h>
#include <eval.h>
#include <exec.h>
#include <fcntl.h>
#include <parser.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils.h>

static void eval_full (ast_t *ast);

static int input_fd = -1;
static int output_fd = -1;
static int bg = 0;

static void eval_commandtail (ast_t *ast) {
    command_t *cmd = command_create ();
    command_argv_from_ast (cmd, ast);
    cmd->input_fd = input_fd;
    cmd->output_fd = output_fd;
    cmd->bg = bg;
    command_execute (cmd);
    command_destroy (cmd);
}
static void eval_sequence (ast_t *ast) {
    if (ast->type == AST_BACKGROUND) {
        bg = 1;
    }

    eval_full (ast->left);
    eval_full (ast->right);

    if (ast->type == AST_BACKGROUND) {
        bg = 0;
    }
}
static void eval_redirection (ast_t *ast) {
    int fd;
    if (ast->type == AST_REDIRECT_IN) {
        fd = open (ast->token->lexeme, O_RDONLY);
        if (fd == -1) {
            psherror ("open");
            return;
        }
        input_fd = fd;
    } else {
        fd = open (ast->token->lexeme, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);
        if (fd == -1) {
            psherror ("open");
            return;
        }
        output_fd = fd;
    }

    eval_full (ast->right);
    close (fd);
    input_fd = -1;
    output_fd = -1;
}
static void eval_pipeline (ast_t *ast) {
    int pipefd[2];
    if (pipe (pipefd) == -1) {
        psherror ("pipe");
        return;
    }

    input_fd = -1;
    output_fd = pipefd[1];
    eval_full (ast->left);
    close (pipefd[1]);

    ast = ast->right;

    input_fd = pipefd[0];

    while (ast->type == AST_PIPE) {
        if (pipe (pipefd) == -1) {
            psherror ("pipe");
            return;
        }
        output_fd = pipefd[1];
        eval_full (ast->left);
        close (pipefd[1]);
        close (input_fd);
        input_fd = pipefd[0];
        ast = ast->right;
    }
    output_fd = -1;
    input_fd = pipefd[0];
    eval_full (ast);
    close (pipefd[0]);
}

static void eval_full (ast_t *ast) {
    if (ast == NULL) {
        return;
    }
    switch (ast->type) {
    case AST_PIPE:
        eval_pipeline (ast);
        break;
    case AST_REDIRECT_IN:
    case AST_REDIRECT_OUT:
        eval_redirection (ast);
        break;
    case AST_SEQUENCE:
    case AST_BACKGROUND:
        eval_sequence (ast);
        break;
    case AST_COMMAND_TAIL:
        eval_commandtail (ast);
        break;
    default:
        break;
    }
}

void evaluate (parser_t *parser) {
    eval_full (parser->ast);
}
