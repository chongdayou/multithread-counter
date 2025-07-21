#ifndef COMPAT_H
#define COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
typedef long ssize_t;
#endif

#endif