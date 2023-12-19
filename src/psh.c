#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct shellBuiltin {
  char *name;
  int (*func)(char **args);
};

// Prototypes

int psh_cd(char **args);
int psh_help(char **args);
int psh_exit(char **args);

struct shellBuiltin builtins[] = {
    {"cd", &psh_cd},
    {"help", &psh_help},
    {"exit", &psh_exit},
};

#define PSH_NUM_BUILTINS sizeof(builtins) / sizeof(struct shellBuiltin)

// builtins

int psh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "psh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("psh");
    }
  }
  return 1;
}

int psh_help(char **args) {
  if (args != NULL) {
    printf("psh: No args needed!");
  }
  printf("proh14's simple shell\n");
  printf("builtins:\n");
  for (size_t i = 0; i < PSH_NUM_BUILTINS; i++) {
    printf("  %s\n", builtins[i].name);
  }
  return 0;
}

int psh_exit(char **args) {
  if (args != NULL) {
    printf("psh: No args needed!");
  }
  exit(0);
  return 0;
}

// builtins

struct shellConfig {
  char promt[30];
};

struct shellConfig config;

char *read_line(void) {
  char *line = NULL;
  size_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
}

char **split_line(char *line) {
  char **tokens = NULL;
  char *token = NULL;
  int bufsize = 0;
  int position = 0;
  tokens = malloc(sizeof(char *));
  token = strtok(line, " \t\r\n\a");
  while (token != NULL) {

    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      if (tokens == NULL) {
        perror("psh");
        exit(EXIT_FAILURE);
      }
      bufsize += 64;
      tokens = realloc(tokens, bufsize * sizeof(char *));
    }

    token = strtok(NULL, " \t\r\n\a");
  }
  tokens[position] = NULL;
  return tokens;
}

int execute(char **args) {
  if (args[0] == NULL) {
    exit(0);
  }
  for (size_t i = 0; i < PSH_NUM_BUILTINS; i++) {
    if (strcmp(args[0], builtins[i].name) == 0) {
      return (*builtins[i].func)(args);
    }
  }
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("psh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("psh");
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

void initShell(void) { strcpy(config.promt, "psh> "); }

int main(void) {
  initShell();
  while (1) {
    printf("%s", config.promt);
    char *line = read_line();
    char **args = split_line(line);
    execute(args);
    free(line);
    free(args);
  }
  return 0;
}
