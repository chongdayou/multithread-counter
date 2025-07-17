#ifndef STRBUFFER_H
#define STRBUFFER_H

#include <stdlib.h>

#define INITIAL_CAPACITY 64

typedef struct StrBuffer {
	char* buffer;
	size_t size;
	size_t capacity;
} StrBuffer;

StrBuffer* strbuffer_make(size_t capacity);
void strbuffer_append(StrBuffer* strbuffer, const char* to_append);
char* strbuffer_get_string(StrBuffer strbuffer);
void strbuffer_free(StrBuffer* strbuffer);
#endif