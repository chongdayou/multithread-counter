#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>
#include <stdbool.h>

#include "../include/common.h"

typedef struct aat_tree AatTree;
typedef struct AatIterator AatIterator;

typedef struct word_counter {
	AatTree* tree;
} Counter;

typedef struct word_counter_iterator {
	AatIterator* tree_iterator;
} CounterIterator;

typedef struct word_counter_pair {
	char word[MAX_WORD_LEN];
	int count;
} CounterPair;

Counter* counter_make();
void counter_into_tree(Counter* counter, FILE* file);
void counter_add_pair(Counter* counter, CounterPair pair);
int counter_search_word_occurence(Counter* counter, const char* word);
char* counter_get_all_pairs(Counter* counter);
void counter_free(Counter* counter);

CounterIterator* counter_iterator_make(Counter* counter);
bool counter_iterator_has_next(CounterIterator* iterator);
CounterPair counter_iterator_next(CounterIterator* iterator);
void counter_iterator_free(CounterIterator* iterator);

#endif