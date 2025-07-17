#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>

typedef struct aat_tree AatTree;

typedef struct word_counter {
	AatTree* tree;
} Counter;

void counter_into_tree(Counter* counter, FILE* file);
int counter_search_word_occurence(Counter* counter, const char* word);

#endif