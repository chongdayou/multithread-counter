#ifndef AAT_H
#define AAT_H

#include <stdbool.h>
#include "../include/stack.h"

// -- opaque struct forward declarations --
typedef struct aat_node {
	void* key;
	void* value;
	int level;
	AatNode* left;
	AatNode* right;
} AatNode;

// -- struct declarations and definitions --
typedef struct aat_tree AatTree;
struct aat_tree {
	AatNode* root;
	AatNode* last;
	AatNode* deleted;
	void* (* key_process) (void*);
	int (* key_compare) (void* key_a, void* key_b);
	char* (* key_to_string) (void*);
	void (* key_free) (void*);
	void* (* value_process) (void*);
	char* (* value_to_string) (void*);
	void (* value_free) (void*);
};

typedef struct AatIterator {
	AatTree* tree;
	NodeStack* iterate_stack;
	bool is_end;
} AatIterator;

// -- global variables --
extern AatNode _aat_bottom_;
extern AatNode* _aat_bottom;

// -- function pointers for generic types --
// require free()
void* raw_to_str_process(void* raw);
// require free()
void* raw_to_int_process(void* raw);
int str_compare(void* a, void* b);
int int_compare(void* a, void* b);
// require free()
char* str_to_string(void* raw);
// require free()
char* int_to_string(void* raw);
void str_free(void* raw);
void int_free(void* raw);

// -- method prototypes --
AatTree* aat_tree_make(
	void* (* key_process) (void*), 
	int (* key_compare) (void* key_a, void* key_b),
	char* (* key_to_string) (void*),
	void (* key_free) (void*), 
	void* (* value_process) (void*),
	char* (* value_to_string) (void*),
	void (* value_free) (void*)
);
void aat_tree_free(AatTree* tree);
bool aat_tree_insert(AatTree* tree, void* raw_key, void* raw_value);
AatNode* aat_tree_search(AatTree* tree, void* raw_key);
bool aat_tree_is_empty(AatTree* tree);
bool aat_tree_exists(AatTree* tree, void* raw_key);
// need free()
void* aat_tree_get_value(AatTree* tree, void* raw_key);
bool aat_tree_set_value(AatTree* tree, void* raw_key, void* new_raw_key);
bool aat_tree_delete(AatTree* tree, void* raw_key);
// Returns a dynamically allocated string; caller must free()
char* aat_tree_inorder_list(AatTree* tree);
// void *att_tree_walk(AatTree* tree, void (*func)(void*));
void* aat_tree_iterator(AatTree* tree);

AatIterator* aat_iterator_make(AatTree* tree_);
bool aat_iterator_has_next(AatIterator* iterator);
AatNode* aat_iterator_next(AatIterator* iterator);
void aat_iterator_free(AatIterator* iterator);

#endif