#pragma once
#ifndef _EXEC_H_
#define _EXEC_H_

#include <ast.h>

typedef struct command {
    int argc;
    char **argv;
    int input_fd;
    int output_fd;
    int bg;
} command_t;

command_t *command_create (void);
void command_execute (command_t *cmd);
void command_argv_from_ast (command_t *cmd, ast_t *ast);
void command_destroy (command_t *cmd);

#endif // ! _EXEC_H_
