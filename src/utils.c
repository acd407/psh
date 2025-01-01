#include <errno.h>
#include <psh.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

void eprintf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

void psherror(const char *msg) {
  eprintf("%s: %s: %s\n", PROGRAM_NAME, msg, strerror(errno));
}

void die(const char *msg) {
  if (errno) {
    psherror(msg);
  }
  exit(EXIT_FAILURE);
}

void *xmalloc(size_t size) {
  void *p = malloc(size);
  if (!p) {
    die("malloc");
  }
  return p;
}

void *xrealloc(void *ptr, size_t size) {
  void *p = realloc(ptr, size);
  if (!p) {
    die("realloc");
  }
  return p;
}

char *xstrdup(const char *s) {
  char *p = malloc(strlen(s) + 1);
  if (!p) {
    die("strdup");
  }
  strcpy(p, s);
  return p;
}
