#ifndef AAT_H
#define AAT_H

#include <stdbool.h>

// -- opaque struct forward declarations --
typedef struct aat_node AatNode;

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
	void (* value_free) (void*)
};

// -- global variables --
extern AatNode _aat_bottom_;
extern AatNode* _aat_bottom;

// -- method prototypes --
AatTree* aat_tree_make();
void aat_tree_free(AatTree* tree);
void aat_tree_insert(AatTree* tree, void* key, void* value);
AatNode* aat_tree_search(AatTree* tree, void* key);
bool aat_tree_exists(AatTree* tree, void* key);
bool aat_tree_delete(AatTree* tree, void* key);
// Returns a dynamically allocated string; caller must free()
char* aat_tree_inorder_print(AatTree* tree);

#endif