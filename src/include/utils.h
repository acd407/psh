#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void *xmalloc (size_t size);
void *xrealloc (void *ptr, size_t size);
char *xstrdup (const char *s);

void eprintf (const char *fmt, ...);
void psherror (const char *msg);
void die (const char *msg);

#endif // !_UTILS_H_
