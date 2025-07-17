#include <stdlib.h>
#include "../include/stack.h"
#include "../include/aat.h"

#define INITIAL_STACK_CAPACITY 16

void stack_init(NodeStack* s) {
    s->capacity = INITIAL_STACK_CAPACITY;
    s->size = 0;
    s->data = malloc(sizeof(AatNode*) * s->capacity);
}

void stack_push(NodeStack* s, AatNode* node) {
    if (s->size >= s->capacity) {
        s->capacity *= 2;
        s->data = realloc(s->data, sizeof(AatNode*) * s->capacity);
    }
    s->data[s->size++] = node;
}

AatNode* stack_pop(NodeStack* s) {
    if (s->size == 0) return NULL;
    return s->data[--s->size];
}

int stack_is_empty(NodeStack* s) {
    return s->size == 0;
}

void stack_free(NodeStack* s) {
    free(s->data);
    s->data = NULL;
    s->size = 0;
    s->capacity = 0;
}