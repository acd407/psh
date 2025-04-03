#include <builtins.h>
#include <stdio.h>
#define _GNU_SOURCE
#include <alloca.h>
#include <dirent.h>
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
    if (!arg[p]) { // only a key
        char *val = getenv (arg);
        if (val)
            puts (val);
        else
            eprintf ("set: empty\n");
    } else {
        char *argkv = alloca (strlen (arg) + 1);
        strcpy (argkv, arg);
        argkv[p++] = '\0';
        if (arg[p]) { // with value
            if (setenv (argkv, &arg[p], 1) == -1)
                perror ("set");
        } else { // have '=' but without value
            if (unsetenv (argkv) == -1)
                perror ("set");
        }
    }
}

void builtin_ls (int argc, char **argv) {
    DIR *d;
    if (argc == 2)
        d = opendir (argv[1]);
    else
        d = opendir (".");
    if (d) {
        struct dirent *dir;
        while ((dir = readdir (d)) != NULL)
            printf ("%s\n", dir->d_name);
        if (closedir (d) == -1)
            perror ("ls");
    } else
        perror ("ls");
}

static builtin_t builtins[] = {
    {"exit", &builtin_exit}, {"cd", &builtin_cd}, {"pwd", &builtin_pwd},
    {"set", &builtin_set},   {"l", &builtin_ls},
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
