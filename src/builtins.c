#include <builtins.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils.h>

static void builtin_exit(int argc, char **argv) {
  (void)argc;
  (void)argv;
  exit(0);
}

static void builtin_cd(int argc, char **argv) {
  if (argc == 1) {
    chdir(getenv("HOME"));
  } else if (argc == 2) {
    if (chdir(argv[1]) == -1) {
      perror("cd");
    }
  } else {
    eprintf("cd: too many arguments\n");
  }
}

static void builtin_pwd(int argc, char **argv) {
  (void)argv;
  if (argc > 1) {
    eprintf("pwd: too many arguments\n");
    return;
  }
  char *cwd = getcwd(NULL, 0);
  printf("%s\n", cwd);
  free(cwd);
}

static builtin_t builtins[] = {
    {"exit", &builtin_exit},
    {"cd", &builtin_cd},
    {"pwd", &builtin_pwd},
};

builtin_t *builtin_find_by_name(char *name) {
  for (size_t i = 0; i < sizeof(builtins) / sizeof(builtin_t); i++) {
    if (strcmp(builtins[i].name, name) == 0) {
      return &builtins[i];
    }
  }
  return NULL;
}

void builtin_execute(builtin_t *builtin, int argc, char **argv) {
  if (builtin != NULL) {
    builtin->func(argc, argv);
  }
}
