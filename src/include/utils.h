#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);

void eprintf(const char *fmt, ...);
void psherror(const char *msg);
void die(const char *msg);

#endif // !_UTILS_H_
