#include <stdio.h>

#include "../include/aat.h"
#include "../include/strbuffer.h"

// TODO: copied from old aat.c, need modification as it is not in aat.c anymore
// 		and inorder print should return char* instead of print something out
int main(int argc, char* argv[]) {
	size_t n = 100;
	int*  v = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		v[i] = i;
	}

	for (int i = 0; i < n; i++) {
		int j = rand() % n;
		int temp = v[i];
		v[i] = v[j];
		v[j] = temp;
	} 

	AatTree* tree = aat_tree_make();
	printf("The list of input by order of insertion: \n");
	for (int i=0; i < n; i++) {
		printf("%d, ", v[i]);
		aat_tree_insert(v[i], tree);
	}
	printf("\n");

	printf("Inorder traversal of the tree: \n");
	aat_tree_inorder_print(tree);
	printf("\n");

	printf("Deleting all multiples of 10 in the tree...\n");
	printf("Inorder traversal after deletions: \n");
	for (int i=0; i < 10; i++) {
		aat_tree_delete((i+1)*10, tree);
	}
	aat_tree_inorder_print(tree);
	printf("\n");
	printf("Deleting all even numbers in the tree..." 
		"this will also test removing none existing values " 
		"as some even numbers are multiples of 10.\n");
	printf("Inorder traversal after deletions: \n");
	for (int i=0; i < n/2; i++) {
		aat_tree_delete((i+1)*2, tree);
	}
	aat_tree_inorder_print(tree);
	printf("\n");

	printf("Building a second tree for testing if bottom node is shared correctly: \n");
	AatTree* secTree = aat_tree_make();
	for (int i=n; i >= 0; i--) {
		aat_tree_insert(i, secTree);
	}
	printf("All power of 2 are deleted in this tree for fun.\n");
	for (int i=1; i<n; i=i*2) {
		aat_tree_delete(i, secTree);
	}
	printf("Inorder traversal of second tree: \n");
	aat_tree_inorder_print(secTree);
	printf("\n");

	aat_tree_free(tree);
	aat_tree_free(secTree);
	free(v);
	return 0;
}