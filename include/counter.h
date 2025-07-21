#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>

typedef struct aat_tree AatTree;

typedef struct word_counter {
	AatTree* tree;
} Counter;

Counter* counter_make();
void counter_into_tree(Counter* counter, FILE* file);
int counter_search_word_occurence(Counter* counter, const char* word);
char* counter_get_all_pairs(Counter* counter);
void counter_free(Counter* counter);

#endif