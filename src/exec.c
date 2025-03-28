#define _DEFAULT_SOURCE
#include <ast.h>
#include <builtins.h>
#include <exec.h>
#include <fcntl.h>
#include <psh.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utils.h>

#include <readline/readline.h>

static void command_reset (command_t *cmd) {
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->input_fd = -1;
    cmd->output_fd = -1;
    cmd->bg = 0;
}

command_t *command_create (void) {
    command_t *cmd = xmalloc (sizeof (command_t));
    command_reset (cmd);
    return cmd;
}

void command_argv_from_ast (command_t *cmd, ast_t *ast) {

    // Prepare argv
    for (ast_t *n = ast; n != NULL; n = n->right) {
        cmd->argc++;
    }
    cmd->argv = xmalloc (sizeof (char *) * (cmd->argc + 1));

    for (int i = 0; i < cmd->argc; i++) {
        cmd->argv[i] = xstrdup (ast->token->lexeme);
        ast = ast->right;
    }
    cmd->argv[cmd->argc] = NULL;
}

static int xopen (const char *path, int flags, mode_t mode) {
    int fd = open (path, flags, mode);
    if (fd == -1) {
        die ("open");
    }
    return fd;
}

static void untrap_signals (void) {
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGTTIN, SIG_DFL);
    signal (SIGTTOU, SIG_DFL);
}

static void background_signal_handler (int signo) {
    if (signo == SIGCHLD) {
        int status;
        pid_t pid;
        while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED (status) || WIFSIGNALED (status)) {
                printf ("[BACKGROUND] Process %d exited\n", pid);
                rl_on_new_line ();
                rl_refresh_line (0, 0);
            }
        }
    }
}

void command_execute (command_t *cmd) {
    if (cmd->argc == 0) {
        return;
    }

    // Builtins
    builtin_t *builtin = builtin_find_by_name (cmd->argv[0]);
    if (builtin) {
        builtin_execute (builtin, cmd->argc, cmd->argv);
        return;
    }

    // Fork
    pid_t pid = fork ();
    if (pid == -1) {
        psherror ("fork");
        return;
    }

    // Child
    if (pid == 0) {
        untrap_signals ();

        if (cmd->bg) {
            printf ("\n[BACKGROUND] started backgroundjob: %s\n", cmd->argv[0]);
            setpgid (0, 0);

            int fd = xopen ("/dev/null", O_RDONLY, 0);
            dup2 (fd, STDIN_FILENO);
            close (fd);
        }

        // Pipe
        if (cmd->input_fd != -1) {
            dup2 (cmd->input_fd, STDIN_FILENO);
            close (cmd->input_fd);
        }
        if (cmd->output_fd != -1) {
            dup2 (cmd->output_fd, STDOUT_FILENO);
            close (cmd->output_fd);
        }

        // Execute
        if (execvp (cmd->argv[0], cmd->argv) == -1) {
            die ("execvp");
        }
    }
    // Wait for child
    if (!cmd->bg) {
        int status;
        do {
            waitpid (pid, &status, WUNTRACED);
        } while (!WIFEXITED (status) && !WIFSIGNALED (status));
        return;
    }

    // Background process
    struct sigaction sa;
    sa.sa_handler = background_signal_handler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Restart interrupted syscalls,
                                             // don't handle stopped children
    sigaction (SIGCHLD, &sa, NULL);
}

void command_destroy (command_t *cmd) {
    if (cmd->argv) {
        for (int i = 0; i < cmd->argc; i++) {
            free (cmd->argv[i]);
        }
        free (cmd->argv);
    }
    command_reset (cmd);
    free (cmd);
    cmd = NULL;
}
