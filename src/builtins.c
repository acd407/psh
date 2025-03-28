#include <builtins.h>
#include <stdio.h>
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils.h>

static void builtin_exit (int argc, char **argv) {
    (void) argc;
    (void) argv;
    exit (0);
}

static void builtin_cd (int argc, char **argv) {
    if (argc == 1) {
        if (chdir (getenv ("HOME")) == -1) {
            perror ("cd");
        }
    } else if (argc == 2) {
        if (chdir (argv[1]) == -1) {
            perror ("cd");
        }
    } else {
        eprintf ("cd: too many arguments\n");
    }
}

static void builtin_pwd (int argc, char **argv) {
    (void) argv;
    if (argc > 1) {
        eprintf ("pwd: too many arguments\n");
        return;
    }
    char *cwd = getcwd (NULL, 0);
    printf ("%s\n", cwd);
    free (cwd);
}

static void builtin_set (int argc, char **argv) {
    if (argc > 2) {
        eprintf ("set: too many arguments\n");
        return;
    }
    if (argc < 2) {
        for (char **env = environ; *env != NULL; env++) {
            printf ("%s\n", *env);
        }
        return;
    }
    char *arg = argv[1];
    int p = 0;
    while (arg[p] && arg[p] != '=')
        p++;
    if (!arg[p]) {
        char *val = getenv (arg);
        if (val)
            puts (val);
        else {
            eprintf ("set: empty value\n");
            return;
        }
        return;
    }
    if (arg[1 + p]) {
        char *argkv = malloc (strlen (arg) + 1);
        strcpy (argkv, arg);
        argkv[p] = '\0';
        setenv (argkv, &arg[p + 1], 1);
        free (argkv);
    }
}

static builtin_t builtins[] = {
    {"exit", &builtin_exit},
    {"cd", &builtin_cd},
    {"pwd", &builtin_pwd},
    {"set", &builtin_set},
};

builtin_t *builtin_find_by_name (char *name) {
    for (size_t i = 0; i < sizeof (builtins) / sizeof (builtin_t); i++) {
        if (strcmp (builtins[i].name, name) == 0) {
            return &builtins[i];
        }
    }
    return NULL;
}

void builtin_execute (builtin_t *builtin, int argc, char **argv) {
    if (builtin != NULL) {
        builtin->func (argc, argv);
    }
}
