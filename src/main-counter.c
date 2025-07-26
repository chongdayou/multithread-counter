#include <stdlib.h>
#include <stdio.h>

#include "../include/counter.h"

int main(int argc, char* argv[]) {
	FILE* file = fopen("text/HamletActIISceneI.txt", "r");
	Counter* counter = counter_make();
	counter_into_tree(counter, file);

	CounterIterator* iterator = counter_iterator_make(counter);
	CounterPair pair;
	while (counter_iterator_has_next(iterator)) {
		pair = counter_iterator_next(iterator);
		printf("(%s,%d), ", pair.word, pair.count);
	}
	printf("\n");

	fclose(file);
	counter_iterator_free(iterator);
	counter_free(counter);

	return 0;
}