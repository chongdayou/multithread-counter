#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../include/stack.h"
#include "../include/aat.h"
#include "../include/strbuffer.h"

// TODO: enforce type safety/type checking? Now everything working with key and value are void*
// 		which requires ethical use from the caller.
// TODO: right now a bunch of function pointers are needed to make a tree, 
// 		find a way to make it easier for the caller?

// -- internal struct definitions --
struct aat_node {
	void* key;
	void* value;
	int level;
	AatNode* left;
	AatNode* right;
};

// -- global variables --
AatNode _aat_bottom_;
AatNode* _aat_bottom = &_aat_bottom_;
static bool bottom_initialized = false;

// -- private method prototypes --
// for AatNode
static void init_bottom();
static AatNode* aat_node_make(
	void* key_, 
	void* value_
);
static void aat_node_free(AatNode* node);
static void rotate_right(AatNode** node_ptr);
static void rotate_left(AatNode** node_ptr);
static void aat_node_skew(AatNode** node_ptr);
static void aat_node_split(AatNode** node_ptr);
// for AatTree
static void aat_tree_free_helper(AatNode* root);
static bool aat_tree_insert_helper(
	AatNode** root, 
	AatTree* tree,
	void* processed_key, 
	void* processed_value
);
static void aat_tree_insert_rebalance(
	AatNode** root, 
	AatTree* tree,
	void* processed_key, 
	void* processed_value
);
static bool aat_tree_delete_helper(AatTree* tree, AatNode** root, void* processed_key);


// -- method implementations --
// for function pointers
void* raw_to_str_process(void* raw) {
	if (!raw) {
		fprintf(stderr, "Null pointer passed to raw_to_int_process().\n");
		exit(1);
	}
	return strdup((char*) raw);
};
void* raw_to_int_process(void* raw) {
	if (!raw) {
		fprintf(stderr, "Null pointer passed to raw_to_int_process().\n");
		exit(1);
	}
	int* ret = malloc(sizeof(int));
	*ret = *(int*)raw;
	return ret;
};
int str_compare(void* a, void* b) {
	return strcmp((char*) a, (char*) b);
};
int int_compare(void* a, void* b) {
	return (*(int*)a > *(int*)b) - (*(int*)a < *(int*)b);
}
char* str_to_string(void* raw) {
	return strdup((char*) raw);
};
char* int_to_string(void* raw) {
	char* buffer = malloc(12);
	if (!buffer) {
		fprintf(stderr, "Heap allocation failed in int_to_string()\n");
		exit(1);
	}
	int processed = *(int*)raw;
	snprintf(buffer, 12, "%d", processed);
	return buffer;
}
void str_free(void* raw) {
	free(raw);
};
void int_free(void* raw) {
	free(raw);
};

// for AatNode

/**
 * Initialize bottom node, shared across multiple trees.
 */
static void init_bottom() {
	_aat_bottom->left = _aat_bottom;
	_aat_bottom->right = _aat_bottom;
	_aat_bottom->level = 0;
}

/**
 * Make a new node. Level of node is initialized to 1.
 * Left and right nodes are set to bottom since new node is assumed to be leaf first.
 * 
 * @param key_ Key of the node
 * @param key_process Function pointer to specify type of key
 * @param key_compare Function pointer to specify how to compare keys between nodes
 * @param key_to_string Function pointer to return key as char*
 * @param key_free Function pointer to free key
 * @param value_ Value of the node
 * @param value_process Function pointer to specify type of value
 * @param value_to_string Function pointer to return value as char*
 * @param value_free Function pointer to free value
 * @return Pointer to a heap allocated AatNode
 */
static AatNode* aat_node_make(
	void* processed_key,
	void* processed_value
) {
	AatNode* node = malloc(sizeof(AatNode));
	node->key = processed_key;
	node->value = processed_value;
	node->level = 1;
	node->left = _aat_bottom;
	node->right = _aat_bottom;
	return node;
}

/**
 * Free the node.
 * 
 * @param node Pointer to a node.
 */
static void aat_node_free(AatNode* node) {
	free(node->key);
	free(node->value);
	free(node);
}

/**
 * Rotate a node to the right of its left child.
 */
static void rotate_right(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->left;
	(*node_ptr)->left = new_parent->right;
	new_parent->right = *node_ptr;
	*node_ptr = new_parent;
}

/**
 * Rotate a node to the left of its right child.
 */
static void rotate_left(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->right;
	(*node_ptr)->right = new_parent->left;
	new_parent->left = *node_ptr;
	*node_ptr = new_parent;
}

/**
 * If the node's left child has the same level as the node, rotate right.
 */
static void aat_node_skew(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->left->level) {
		rotate_right(node_ptr);
	}
}

/**
 * If the node's right right child has the same level as the node, rotate the node to the left of its right node.
 * This will result in the node between the node and its right right child be the new parent of both nodes.
 * Then increment the level of the new parent by 1.
 */
static void aat_node_split(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->right->right->level) {
		rotate_left(node_ptr);
		(*node_ptr)->level++;
	}
}

// for AatTree
/**
 * Make a new AatTree where the type of nodes' keys and values are specified with function pointers.
 * 
 * @param key_process Function pointer to specify type of key
 * @param key_compare Function pointer to specify how to compare keys between nodes
 * @param key_to_string Function pointer to return key as char*
 * @param key_free Function pointer to free key
 * @param value_process Function pointer to specify type of value
 * @param value_to_string Function pointer to return value as char*
 * @param value_free Function pointer to free value
 * @return Pointer to a heap allocated AatTree
 */
AatTree* aat_tree_make(
	void* (* key_process) (void*), 
	int (* key_compare) (void* key_a, void* key_b),
	char* (* key_to_string) (void*),
	void (* key_free) (void*), 
	void* (* value_process) (void*),
	char* (* value_to_string) (void*),
	void (* value_free) (void*)
) {
	AatTree* tree = malloc(sizeof(AatTree));
	if (!bottom_initialized) init_bottom();
	tree->root = _aat_bottom;
	tree->last = _aat_bottom;
	tree->deleted = _aat_bottom;

	tree->key_process = key_process;
	tree->key_compare = key_compare;
	tree->key_to_string = key_to_string;
	tree->key_free = key_free;
	tree->value_process = value_process;
	tree->value_to_string = value_to_string;
	tree->value_free = value_free;

	return tree;
}

/**
 * Private helper method to free all nodes in a tree.
 * 
 * @param root root node of the tree
 */
static void aat_tree_free_helper(AatNode* root) {
	if (root == NULL || root == _aat_bottom) return;
	aat_tree_free_helper(root->left);
	aat_tree_free_helper(root->right);
	aat_node_free(root);
}

/**
 * Free the entire tree, including all its nodes.
 * 
 * @param tree tree to be freed
 */
void aat_tree_free(AatTree* tree) {
	aat_tree_free_helper(tree->root);
	free(tree);
}

/**
 * Helper method to find correct path to leaf for insertion and rebalance on the way back to root.
 * This method is meant to be recursively called, each call represents a step to the leaf where 
 * insertion should happen. After insertion, this method will follow the path from the leaf back up 
 * to the root, and rebalance at each point of the path if needed.
 * 
 * @param root Current node to be examined
 * @param tree Tree to insert new node into
 * @param raw_key Key of the new node
 * @param raw_value Value of the new node
 * 
 * @note If the current node (root) has the same key as key to be inserted, 
 * 		call stack will end and nothing will be inserted into the tree.
 */
static void aat_tree_insert_rebalance(
	AatNode** root, 
	AatTree* tree,
	void* raw_key, 
	void* raw_value
) {
	void* processed_key = tree->key_process(raw_key);
	int key_cmp = tree->key_compare(processed_key, (*root)->key);
	if (key_cmp < 0) {
		aat_tree_insert_helper(
			&(*root)->left,
			tree,
			raw_key,
			raw_value
		);
	} else if (key_cmp > 0) {
		aat_tree_insert_helper(
			&(*root)->right,
			tree,
			raw_key,
			raw_value
		);
	} else {
		// this happens when a node with exact same key is found in tree
		// do nothing, this will end the call stack and nothing will be inserted into tree
	}
	free(processed_key);
	aat_node_skew(root);
	aat_node_split(root);
}

/**
 * Helper method for aat_tree_insert(). Make a new node and perform insertion if root == bottom.
 * If not at bottom yet, call aat_tree_insert_rebalance(). aat_tree_insert_rebalance() will then 
 * call this method on the next node in the correct path to the leaf node where insertion should 
 * happen. This recursive call stack ends when insertion actually happens.
 * 
 * @param root Current node to be examined
 * @param tree Tree to insert new node into
 * @param raw_key Key of the new node
 * @param raw_value Value of the new node
 * 
 * @return true when a node is inserted, false when no node is inserted because the call stack 
 * 		is ended by aat_tree_insert_rebalance() when a node with the same key is found in the tree
 */
static bool aat_tree_insert_helper(
	AatNode** root, 
	AatTree* tree,
	void* raw_key, 
	void* raw_value
) {
	if (*root == _aat_bottom) {
		void* processed_key = tree->key_process(raw_key);
		void* processed_value = tree->value_process(raw_value);
		*root = aat_node_make(
			processed_key,
			processed_value
		);
		return true;
	} else {
		aat_tree_insert_rebalance(
			root,
			tree,
			raw_key,
			raw_value
		);
	}
	return false;
}

/**
 * Insert a new node into the tree.
 * 
 * @param tree Tree where new node is inserted to
 * @param key Key of the new node
 * @param value Value of the new node
 * 
 * @return true if a node is inserted, false if a node with same key is found 
 * 		and no insertion happened
 */
bool aat_tree_insert(AatTree* tree, void* raw_key, void* raw_value) {
	if (!tree) {
        fprintf(stderr, "Null tree passed into aat_tree_insert().\n");
        exit(1);
    }
    if (!raw_key) {
        fprintf(stderr, "Null key passed into aat_tree_insert().\n");
        exit(1);
    }
    if (!raw_value) {
        fprintf(stderr, "Null key passed into aat_tree_insert().\n");
        exit(1);
    }

	return aat_tree_insert_helper(
		&tree->root, 
		tree,
		raw_key,
		raw_value
	);
}

/**
 * Search for a node in the tree that has the key.
 * 
 * @param tree Tree to search in
 * @param key Key of the node to search for
 * 
 * @return Pointer to AatNode that contains the key, NULL if key is not found.
 * 
 * @note If either tree or key is NULL, an error will be print to stderr and 
 * 		the program will exist with exit(1).
 */
AatNode* aat_tree_search(AatTree* tree, void* raw_key) {
	if (tree == NULL || raw_key == NULL) {
		fprintf(stderr, "Invalid NULL pointer.");
		exit(1);
	}
	if (aat_tree_is_empty(tree)) return NULL;

	void* processed_key = tree->key_process(raw_key);
	AatNode* current = tree->root;
	while (current != _aat_bottom) {
		int key_cmp = tree->key_compare(current->key, processed_key);
		if (key_cmp == 0) {
			free(processed_key);
			return current;
		}
		if (key_cmp < 0) current = current->right;
		else current = current->left;
	}
	free(processed_key);

	return current;
}

/**
 * Check if a given tree is empty.
 * 
 * @param tree Tree to check
 * 
 * @return true if tree does not have any node (except _aat_bottom), false otherwise
 */
bool aat_tree_is_empty(AatTree* tree) {
	return tree->root == _aat_bottom;
}

/**
 * Find out if a node with the key exists in the tree.
 * 
 * @param tree Tree to search in
 * @param key Key of the node to search for
 * 
 * @return True if the tree contains a node with the key, false otherwise.
 * 
 * @note If either tree or key is NULL, an error will be print to stderr and 
 * 		the program will exist with exit(1).
 */
bool aat_tree_exists(AatTree* tree, void* raw_key) {
	if (tree == NULL || raw_key == NULL) {
		fprintf(stderr, "Invalid NULL pointer.");
		exit(1);
	}
	if (aat_tree_is_empty(tree)) return false;

	void* processed_key = tree->key_process(raw_key);
	AatNode* current = tree->root;
	while (current != _aat_bottom) {
		int key_cmp = tree->key_compare(current->key, processed_key);
		if (key_cmp == 0) {
			free(processed_key);
			return true;
		}
		if (key_cmp < 0) current = current->right;
		else current = current->left;
	}
	free(processed_key);

	return false;
}

/**
 * Get value from a node with given key in the tree.
 * 
 * @param tree Tree to find the node from
 * @param raw_key Key of the node to get value from
 * 
 * @return Value of the node if the node exists, NULL otherwise
 * 
 * @note if either tree or raw_key is invalid, error will be thrown to stderr 
 * 		and program will exit with exit(1).
 */
void* aat_tree_get_value(AatTree* tree, void* raw_key) {
	if (!tree) {
		fprintf(stderr, "Invalid tree passed into aat_tree_get_value().\n");
		exit(1);
	}
	if (!raw_key) {
		fprintf(stderr, "Invalid key passed into aat_tree_get_value().\n");
		exit(1);
	}
	if (!aat_tree_exists(tree, raw_key)) return NULL;

	AatNode* node = aat_tree_search(tree, raw_key);
	void* value = tree->value_process(node->value);
	return value;
}

/**
 * Find a node inside the tree with given key, and set the node's value to the new value.
 * 
 * @param tree Tree to find the node from
 * @param raw_key Key of the node that needs to change value
 * @param new_raw_value New value that the node will get
 * 
 * @return true if the new value is successfully set, false if node with given key does not exist
 * 
 * @note if tree, raw_key, or new_raw_value is invalid, error will be thrown to stderr 
 * 		and program will exit with exit(1).
 */
bool aat_tree_set_value(AatTree* tree, void* raw_key, void* new_raw_value) {
	if (!tree) {
		fprintf(stderr, "Invalid tree passed into aat_tree_set_value().\n");
		exit(1);
	}
	if (!raw_key) {
		fprintf(stderr, "Invalid key passed into aat_tree_set_value().\n");
		exit(1);
	}
	if (!new_raw_value) {
		fprintf(stderr, "Invalid value passed into aat_tree_set_value().\n");
		exit(1);
	}

	if (!aat_tree_exists(tree, raw_key)) return false;

	void* processed_key = tree->key_process(raw_key);
	AatNode* node = aat_tree_search(tree, processed_key);
	tree->value_free(node->value);
	node->value = tree->value_process(new_raw_value);
	free(processed_key);
	return true;
}

/**
 * Helper method for aat_tree_delete(). Called recursively to find and remove the node with given key.
 * Restructure the tree where needed along the call path from leaf to root.
 * 
 * @param tree Tree to delete the node from
 * @param root Current node the method is working on
 * @param processed_key Key of the node to delete from. Must be processed by key_process function 
 * 		pointed in the tree.
 * 
 * @return at the end of the call stack, return true if a node is deleted, false otherwise
 */
static bool aat_tree_delete_helper(AatTree* tree, AatNode** root, void* processed_key) {
	// base case, reach _aat_bottom
	if (*root == _aat_bottom) return false;

	bool success = false;
	// search down the tree
	tree->last = *root;
	if (tree->key_compare(processed_key, (*root)->key) < 0) {
		success = aat_tree_delete_helper(tree, &(*root)->left, processed_key);
	}
	else {
		tree->deleted = *root;
		success = aat_tree_delete_helper(tree, &(*root)->right, processed_key);
	}

	// remove
	if (
		(*root == tree->last) && 
		(tree->deleted != _aat_bottom) && 
		(tree->key_compare(processed_key, tree->deleted->key) == 0)
	) {
		// to prevent use-after-free when root == deleted
		// key_process and value_process are called before key_free and value_free
		void* new_key = tree->key_process((*root)->key);
		void* new_value = tree->value_process((*root)->value);
		tree->key_free(tree->deleted->key);
		tree->deleted->key = new_key;
		tree->value_free(tree->deleted->value);
		tree->deleted->value = new_value;
		tree->deleted = _aat_bottom;
		*root = (*root)->right;
		aat_node_free(tree->last);
		success = true;
	}

	// rebalance on the way back
	else if (
		((*root)->left->level < (*root)->level - 1) || 
		((*root)->right->level < (*root)-> level - 1)
	) {
		(*root)->level--;
		if ((*root)->right->level > (*root)->level) (*root)->right->level--;
		aat_node_skew(root);
		aat_node_skew(&(*root)->right);
		aat_node_skew(&(*root)->right->right);
		aat_node_split(root);
		aat_node_split(&(*root)->right);
	}

	return success;
}

/**
 * Delete a node with the given key.
 * 
 * @param tree Tree that the node should delete from
 * @param raw_key void* key to be deleted
 * 
 * @return true if a node is found and deleted, false otherwise
 */
bool aat_tree_delete(AatTree* tree, void* raw_key) {
	void* processed_key = tree->key_process(raw_key);
	bool success =  aat_tree_delete_helper(tree, &tree->root, processed_key);
	free(processed_key);
	return success;
}

/**
 * In-order traversal of the tree. Each node is represented as pair (key:value).
 * 
 * @param tree Tree to traverse
 * 
 * @return String of keys of nodes in in-order traversal of the tree
 */
char* aat_tree_inorder_list(AatTree* tree) {
	// using a stack implementation that is ChatGPT-generated for inorder print
	NodeStack stack;
	stack_init(&stack);
	StrBuffer* sb = strbuffer_make(INITIAL_CAPACITY);

	AatNode* current = tree->root;

	while (!stack_is_empty(&stack) || current != _aat_bottom) {
		while (current != _aat_bottom) {
			stack_push(&stack, current);
			current = current->left;
		}
		current = stack_pop(&stack);
		strbuffer_append(sb, "(");
		char* key_str = tree->key_to_string(current->key);
		strbuffer_append(sb, key_str);
		free(key_str);
		strbuffer_append(sb, ":");
		char* value_str = tree->value_to_string(current->value);
		strbuffer_append(sb, value_str);
		free(value_str);
		//if (!stack_is_empty(&stack)) 
		strbuffer_append(sb, "), ");
		current = current->right;
	}

	char* ret = strdup(strbuffer_get_string(*sb));
	stack_free(&stack);
	strbuffer_free(sb);
	return ret;
}