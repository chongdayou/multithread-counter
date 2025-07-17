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
};

// -- global variables --
extern AatNode _aat_bottom_;
extern AatNode* _aat_bottom;

// -- method prototypes --
AatTree* aat_tree_make();
void aat_tree_free(AatTree* tree);
void aat_tree_insert(int key_, AatTree* tree);
bool aat_tree_delete(int key_, AatTree* tree);
// Returns a dynamically allocated string; caller must free()
char* aat_tree_inorder_print(AatTree* tree);

#endif