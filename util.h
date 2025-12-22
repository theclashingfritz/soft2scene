#pragma once

#include <stdio.h>
#include <Windows.h>

#include "types.h"

#ifdef _DEBUG
  #define dprintf(format, ...) printf(format, __VA_ARGS__);
#else
  #define dprintf(format, ...)
#endif

#define lprintf(format, verbose_level, ...) if (verbose >= verbose_level) { printf(format, __VA_ARGS__); }
#define lfprintf(file, format, verbose_level, ...) if (verbose >= verbose_level) { fprintf(file, format, __VA_ARGS__); }

extern int verbose;
extern FILE *log_file;

// General

void safe_exit(int code);