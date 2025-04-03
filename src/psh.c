#include <eval.h>
#include <lexer.h>
#include <limits.h>
#include <parser.h>
#include <psh.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>

#include <readline/history.h>
#include <readline/readline.h>

static void lexical_error (lexical_error_t *e) {
    if (e == NULL) {
        return;
    }
    eprintf ("%s: %s: at character %d\n", PROGRAM_NAME, e->msg, e->character);
}

static void parsing_error (parsing_error_t *e) {
    if (e == NULL) {
        return;
    }
    eprintf ("%s: %s: '%s'\n", PROGRAM_NAME, e->msg, e->bad_tok);
}

static void trap_signals (void) {
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
}

static void do_input (char *input) {
    lexer_t *lexer = lexer_create ();
    if (lex (lexer, input) == NULL) {
        lexical_error (lexer->error);
        lexer_destroy (lexer);
        return;
    }

    parser_t *parser = parser_create ();

    if (parse (parser, lexer) == NULL) {
        parsing_error (parser->err);

        parser_destroy (parser);
        lexer_destroy (lexer);
        return;
    }

    evaluate (parser);

    parser_destroy (parser);
    lexer_destroy (lexer);
}

void initrc (char *rc_file) {
    FILE *script = fopen (rc_file, "r");
    if (!script) {
        return;
    }

    char *line = NULL;
    size_t len = 0;
    while (getline (&line, &len, script) != -1) {
        // 移除行尾换行符
        line[strcspn (line, "\n")] = '\0';

        do_input (line);
    }

    free (line);
    fclose (script);
}

int main (int argc, char **argv) {
    trap_signals ();

    // 初始化 rc 文件
    char rc_path[PATH_MAX];
    const char *home = getenv ("HOME");
    if (home) {
        snprintf (rc_path, sizeof (rc_path), "%s/.pshrc", home);
        initrc (rc_path);
    }
    initrc ("/etc/pshrc");

    bool interactive_after_script = false;
    const char *script_path = NULL;

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-i") == 0) {
            interactive_after_script = true;
        } else if (!script_path) {
            script_path = argv[i]; // 第一个非选项参数视为脚本路径
        } else {
            eprintf ("%s: too many arguments\n", PROGRAM_NAME);
            exit (EXIT_FAILURE);
        }
    }

    // 执行脚本（如果指定了路径）
    if (script_path) {
        FILE *script = fopen (script_path, "r");
        if (!script) {
            eprintf (
                "%s: cannot open script '%s'\n", PROGRAM_NAME, script_path
            );
            exit (EXIT_FAILURE);
        }

        char *line = NULL;
        size_t len = 0;
        while (getline (&line, &len, script) != -1) {
            line[strcspn (line, "\n")] = '\0';
            do_input (line);
        }

        free (line);
        fclose (script);

        // 如果没有 -i 选项，直接退出
        if (!interactive_after_script) {
            exit (EXIT_SUCCESS);
        }
    }

    // 进入交互模式
    while (1) {
        char *input = readline (PROMPT);
        if (!input)
            exit (0);

        add_history (input);
        do_input (input);
        free (input);
    }
}
