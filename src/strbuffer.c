#include <stdio.h>
#include <string.h>

#include "../include/strbuffer.h"

// -- private prototypes --
static void resize_if_needed(char** buffer, size_t new_size, size_t* capacity);

// -- method definitions --
StrBuffer* strbuffer_make(size_t capacity) {
	StrBuffer* sb = malloc(sizeof(StrBuffer));
	if (!sb) {
		fprintf(stderr, "Failed to allocate StrBuffer.\n");
		exit(1);
	}
	sb->buffer = malloc(capacity);
	if (!sb->buffer) {
		fprintf(stderr, "Failed to allocate StrBuffer's buffer.\n");
		free(sb);
		exit(1);
	}
	sb->buffer[0] = '\0';
	sb->size = 0;
	sb->capacity = capacity;
	return sb;
};

void strbuffer_append(StrBuffer* strbuffer, const char* to_append) {
	const size_t len = strlen(to_append);
	resize_if_needed(&strbuffer->buffer, strbuffer->size + len, &strbuffer->capacity);
	snprintf(strbuffer->buffer + strbuffer->size, strbuffer->capacity - strbuffer->size, "%s", to_append);
	strbuffer->size += len;
};

static void resize_if_needed(char** buffer, size_t new_size, size_t* capacity) {
	while (new_size + 1 > *capacity) {
		*capacity *= 2;
		*buffer = realloc(*buffer, *capacity);
		if (!*buffer) {
			fprintf(stderr, "Failed realloc StrBuffer's buffer.\n");
		}
	}
};

char* strbuffer_get_string(StrBuffer strbuffer) {
	return strbuffer.buffer;
};

void strbuffer_free(StrBuffer* strbuffer) {
	free(strbuffer->buffer);
	free(strbuffer);
};