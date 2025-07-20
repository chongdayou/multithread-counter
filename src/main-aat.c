#include <stdio.h>
#include <stdlib.h>

#include "../include/aat.h"

int main(int argc, char* argv[]) {
	size_t n = 100;
	char* traversal_result;
	int*  v = malloc(n * sizeof(int));
	int* values = calloc(n, sizeof(int));
	for (int i = 0; i < n; i++) {
		v[i] = i;
	}

	for (int i = 0; i < n; i++) {
		int j = rand() % n;
		int temp = v[i];
		v[i] = v[j];
		v[j] = temp;
	} 

	AatTree* tree = aat_tree_make(
		raw_to_int_process,
		int_compare,
		int_to_string,
		str_free,
		raw_to_int_process,
		int_to_string,
		int_free
	);
	printf("The list of input by order of insertion: \n");
	for (int i=0; i < n; i++) {
		printf("%d, ", v[i]);
		aat_tree_insert(tree, &v[i], &values[i]);
	}
	printf("\n");

	printf("Inorder traversal of the tree: \n");
	traversal_result = aat_tree_inorder_print(tree);
	printf("%s", traversal_result);
	free(traversal_result);
	printf("\n");

	printf("Deleting all multiples of 10 in the tree...\n");
	printf("Inorder traversal after deletions: \n");
	for (int i=0; i < 10; i++) {
		aat_tree_delete(tree, &(int){(i+1)*10});
	}
	traversal_result = aat_tree_inorder_print(tree);
	printf("%s", traversal_result);
	free(traversal_result);
	printf("\n");
	printf("Deleting all even numbers in the tree..." 
		"this will also test removing none existing values " 
		"as some even numbers are multiples of 10.\n");
	printf("Inorder traversal after deletions: \n");
	for (int i=0; i < n/2; i++) {
		aat_tree_delete(tree, &(int){(i+1)*2});
	}
	traversal_result = aat_tree_inorder_print(tree);
	printf("%s", traversal_result);
	free(traversal_result);
	printf("\n");

	printf("Building a second tree for testing if bottom node is shared correctly.\n");
	AatTree* secTree = aat_tree_make(
		raw_to_int_process,
		int_compare,
		int_to_string,
		str_free,
		raw_to_int_process,
		int_to_string,
		int_free
	);
	for (int i=n-1; i >= 0; i--) {
		int temp = i;
		//printf("key=%d, value=%d\n", temp, values[i]);
		aat_tree_insert(secTree, &temp, &values[i]);
	}
	printf("Inorder traversal of second tree: \n");
	traversal_result = aat_tree_inorder_print(secTree);
	printf("%s", traversal_result);
	free(traversal_result);
	printf("\n");
	printf("All power of 2 are deleted in this tree for fun.\n");
	for (int i=1; i<n; i=i*2) {
		aat_tree_delete(secTree, &(int){i});
	}
	printf("Inorder traversal of second tree after deletions: \n");
	traversal_result = aat_tree_inorder_print(secTree);
	printf("%s", traversal_result);
	free(traversal_result);
	printf("\n");

	aat_tree_free(tree);
	aat_tree_free(secTree);
	free(v);
	free(values);

	return 0;
}