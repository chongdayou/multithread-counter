#include <stdlib.h>
#include <stdio.h>

#include "../include/counter.h"

int main(int argc, char* argv[]) {

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		fprintf(stderr, "Failed to open file.\n");
		exit(1);
	}

	Counter* counter = counter_make();
	counter_into_tree(counter, file);
	fclose(file);

	int hamlet_occurence = counter_search_word_occurence(counter, "hamlet");
	printf("The word \"hamlet\" appeared this many times: %d.\n", hamlet_occurence);

	char* hamlet_pairs = counter_get_all_pairs(counter);
	printf("Each word and the number it appeared in file: \n%s\n\n", hamlet_pairs);

	free(hamlet_pairs);
	counter_free(counter);

	return 0;
}