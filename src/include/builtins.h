#pragma once
#ifndef _BUILTINS_H_
#define _BIULTINS_H_

typedef struct builtin {
    char *name;
    void (*func) (int argc, char **argv);
} builtin_t;

builtin_t *builtin_find_by_name (char *name);
void builtin_execute (builtin_t *builtin, int argc, char **argv);

#endif
