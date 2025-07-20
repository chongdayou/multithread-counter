#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/counter.h"
#include "../include/aat.h"

/**
 * Construct a counter that works on an AatTree, with key=word and value=occurence.
 * 
 * @return Pointer to a new Counter
 */
Counter* counter_make() {
	Counter* counter = malloc(sizeof(Counter));
	AatTree* tree = aat_tree_make(
		raw_to_str_process,
		str_compare,
		str_to_string,
		str_free,
		raw_to_int_process,
		int_to_string,
		int_free
	);
	counter->tree = tree;
	return counter;
}

void counter_into_tree(Counter* counter, FILE* file) {
	if (!file) {
		fprintf(stderr, "Invalid file passed into counter_into_tree().");
		exit(1);
	}
}

int counter_search_word_occurence(Counter* counter, const char* word) {}

/**
 * Free the counter.
 * 
 * @param counter Counter to be freed
 */
void counter_free(Counter* counter) {
	aat_tree_free(counter->tree);
	free(counter);
}