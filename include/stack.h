#ifndef STACK_H
#define STACK_H

typedef struct aat_node AatNode; 

typedef struct {
    AatNode** data;
    int size;
    int capacity;
} NodeStack;

void stack_init(NodeStack* s);
void stack_push(NodeStack* s, AatNode* node);
AatNode* stack_pop(NodeStack* s);
int stack_is_empty(NodeStack* s);
void stack_free(NodeStack* s);

#endif