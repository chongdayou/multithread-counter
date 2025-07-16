#ifndef STACK_H
#define STACK_H

// Just a forward declaration — no typedef
struct AatNode;

typedef struct {
    struct AatNode** data;
    int size;
    int capacity;
} NodeStack;

void stack_init(NodeStack* s);
void stack_push(NodeStack* s, struct AatNode* node);
struct AatNode* stack_pop(NodeStack* s);
int stack_is_empty(NodeStack* s);
void stack_free(NodeStack* s);

#endif