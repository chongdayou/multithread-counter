#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../include/stack.h"
#include "../include/aat.h"
#include "../include/strbuffer.h"

// -- internal struct definitions --
struct aat_node {
	int key;
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
static AatNode* aat_node_make(int key_);
static void aat_node_free(AatNode* node_ptr);
static void rotate_right(AatNode** node_ptr);
static void rotate_left(AatNode** node_ptr);
static void aat_node_skew(AatNode** node_ptr);
static void aat_node_split(AatNode** node_ptr);
// for AatTree
static void aat_tree_free_helper(AatNode* root);
static void aat_tree_insert_helper(int key_, AatNode** root);
static void aat_tree_insert_rebalance(int key_, AatNode** root);
static bool aat_tree_delete_helper(int key_, AatTree* tree, AatNode** root);


// -- method implementations --
// for AatNode
static void init_bottom() {
	_aat_bottom->left = _aat_bottom;
	_aat_bottom->right = _aat_bottom;
	_aat_bottom->level = 0;
	_aat_bottom->key = 0;
}

static AatNode* aat_node_make(int key_) {
	AatNode* node = malloc(sizeof(AatNode));
	node->key = key_;
	node->level = 1;
	node->left = _aat_bottom;
	node->right = _aat_bottom;
	return node;
}

static void aat_node_free(AatNode* node_ptr) {
	free(node_ptr);
}

static void rotate_right(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->left;
	(*node_ptr)->left = new_parent->right;
	new_parent->right = *node_ptr;
	*node_ptr = new_parent;
}

static void rotate_left(AatNode** node_ptr) {
	AatNode* new_parent;
	new_parent = (*node_ptr)->right;
	(*node_ptr)->right = new_parent->left;
	new_parent->left = *node_ptr;
	*node_ptr = new_parent;
}

static void aat_node_skew(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->left->level) {
		rotate_right(node_ptr);
	}
}

static void aat_node_split(AatNode** node_ptr) {
	if ((*node_ptr)->level == (*node_ptr)->right->right->level) {
		rotate_left(node_ptr);
		(*node_ptr)->level++;
	}
}

// for AatTree
AatTree* aat_tree_make() {
	AatTree* tree = malloc(sizeof(AatTree));
	if (!bottom_initialized) init_bottom();
	tree->root = _aat_bottom;
	tree->last = _aat_bottom;
	tree->deleted = _aat_bottom;

	return tree;
}

static void aat_tree_free_helper(AatNode* root) {
	if (root == NULL || root == _aat_bottom) return;
	aat_tree_free_helper(root->left);
	aat_tree_free_helper(root->right);
	free(root);
}

void aat_tree_free(AatTree* tree) {
	aat_tree_free_helper(tree->root);
	free(tree);
}

static void aat_tree_insert_rebalance(int key_, AatNode** root) {
	if (key_ < (*root)->key) aat_tree_insert_helper(key_, &(*root)->left);
	else aat_tree_insert_helper(key_, &(*root)->right);
	aat_node_skew(root);
	aat_node_split(root);
}

static void aat_tree_insert_helper(int key_, AatNode** root) {
	if (*root == _aat_bottom) {
		*root = aat_node_make(key_);
	} else {
		aat_tree_insert_rebalance(key_, root);
	}
}

void aat_tree_insert(int key_, AatTree* tree) {
	aat_tree_insert_helper(key_, &tree->root);
}

static bool aat_tree_delete_helper(int key_, AatTree* tree, AatNode** root) {
	// base case, reach _aat_bottom
	if (*root == _aat_bottom) return false;

	bool success = false;
	// search down the tree
	tree->last = *root;
	if (key_ < (*root)->key) {
		success = aat_tree_delete_helper(key_, tree, &(*root)->left);
	}
	else {
		tree->deleted = *root;
		success = aat_tree_delete_helper(key_, tree, &(*root)->right);
	}

	// remove
	if (
		(*root == tree->last) && 
		(tree->deleted != _aat_bottom) && 
		(key_ == tree->deleted->key)
	) {
		tree->deleted->key = (*root)->key;
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

bool aat_tree_delete(int key_, AatTree* tree) {
	return aat_tree_delete_helper(key_, tree, &tree->root);
}

// using a stack implementation that is ChatGPT-generated for inorder print
char* aat_tree_inorder_print(AatTree* tree) {
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
		char temp[32];
		snprintf(temp, sizeof(temp), "%d, ", current->key);
		strbuffer_append(sb, temp);
		current = current->right;
	}

	char* ret = strdup(strbuffer_get_string(*sb));
	stack_free(&stack);
	strbuffer_free(sb);
	return ret;
}