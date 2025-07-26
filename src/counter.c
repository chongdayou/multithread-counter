#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "../include/counter.h"
#include "../include/aat.h"
#include "../include/compat.h"
#include "../include/common.h"

static void add_to_tree(AatTree* tree, char* key);
static char* clean_tolower_word(char* word);

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

/**
 * Add the occurence of each word in the file into the counter. All words are counted 
 * as their lower case version.
 * 
 * @param counter Counter that will keep track of occurence of each word
 * @param file Source of words to be counted by the counter
 */
void counter_into_tree(Counter* counter, FILE* file) {
	if (!file) {
		fprintf(stderr, "Invalid file passed into counter_into_tree().");
		exit(1);
	}

	char* line = NULL;
	size_t line_len = 0;
	ssize_t read_size;

	while ((read_size = getline(&line, &line_len, file)) != -1) {
		if (line[read_size-1] == '\n') {
			line[read_size-1] = '\0';
		}

		char* token_ptr;
		char* word_token = strtok_r(line, " ", &token_ptr);
		while (word_token) {
			char* cleaned_tolowered_word = clean_tolower_word(word_token);
			//printf("Word to be inserted: %s. \n", cleaned_tolowered_word);
			add_to_tree(counter->tree, cleaned_tolowered_word);
			// it is enough to just pass line into strtok_r() once
			// using NULL here tells strtok_r() to update token_ptr to the next work in line
			word_token = strtok_r(NULL, " ", &token_ptr);
			free(cleaned_tolowered_word);
		}
	}

	free(line);
}

/**
 * Add a pair (key,value) into the tree in the given counter
 * 
 * @param counter Counter to add pair into
 * @param pair Pair that contains the new key and value
 */
void counter_add_pair(Counter* counter, CounterPair pair) {
	if (aat_tree_exists(counter->tree, pair.word)) {
		int* val = (int*) aat_tree_get_value(counter->tree, pair.word);
		int new_val = *val + pair.count;
		aat_tree_set_value(counter->tree, pair.word, &new_val);
		free(val);
	} else {
		aat_tree_insert(counter->tree, pair.word, &pair.count);
	}
}

/**
 * Change the word to all lower case and remove potential punctuations. If '\'' is found and the next char 
 * is not 's', then both will be included.
 * 
 * @param word Word to be processed
 * 
 * @return New word that is lowercased and punctuations removed
 */
static char* clean_tolower_word(char* word) {
	size_t word_len = strlen(word);
	char* new_word = malloc(word_len + 1);
	int old_word_index = 0, new_word_index = 0;

	while (old_word_index < word_len) {
		if (isalnum((unsigned char)word[old_word_index])) {
			new_word[new_word_index++] = tolower((unsigned char)word[old_word_index]);
		}
		// else if (word[old_word_index] == '\'' && old_word_index < word_len && word[old_word_index+1] != 's') {
		// 	new_word[new_word_index++] = word[old_word_index];
		// 	new_word[new_word_index++] = tolower((unsigned char)word[++old_word_index]);
		// }
		old_word_index++;
	}

	new_word[new_word_index] = '\0';

	return new_word;
}

/**
 * If node with key already exist in tree, add 1 to its value. If not, insert a new node 
 * 		with key=key and value=1.
 * 
 * @param tree Tree to update.
 * @param key Key of the node.
 */
static void add_to_tree(AatTree* tree, char* key) {
	if (aat_tree_exists(tree, key)) {
		int* old_value = aat_tree_get_value(tree, key);
		int* new_value = malloc(sizeof(int));
		*new_value = *old_value + 1;
		//printf("[DEBUG] new value updated: %d\n", *new_value);
		//printf("\t[DEBUG] old value got: %d\n", *old_value);
		aat_tree_set_value(tree, key, new_value);
		free(old_value);
		free(new_value);
	} else {
		int* value = malloc(sizeof(int));
		*value = 1;
		//printf("[DEBUG] new value added: %d\n", *value);
		aat_tree_insert(tree, key, value);
		free(value);
	}
}

/**
 * Get how many times a word is counted in the counter.
 * 
 * @param counter Counter to find the word from
 * @param word The word to find
 * 
 * @return Number of times the word is counted in the counter
 * 
 * @note If counter or word is invalid, error will be thrown to stderr 
 * 		and program will exit with exit(1).
 */
int counter_search_word_occurence(Counter* counter, const char* word) {
	if (!counter) {
		fprintf(stderr, "Invalid counter passed into counter_search_word_occurence().\n");
		exit(1);
	}
	if (!word) {
		fprintf(stderr, "Invalid word passed into counter_search_word_occurence().\n");
		exit(1);
	}

	if (!aat_tree_exists(counter->tree, (void*)word)) return 0;
	int* occurence_ptr = aat_tree_get_value(counter->tree, (void*)word);
	int occurence = *occurence_ptr;
	free(occurence_ptr);
	return occurence;
}

/**
 * Get a list of all (word:occurence) in the counter.
 * 
 * @param counter Counter to get list from
 * 
 * @note If counter is invalid, error will be thrown to stderr and program will exit(1).
 */
char* counter_get_all_pairs(Counter* counter) {
	if (!counter) {
		fprintf(stderr, "Invalid counter passed into counter_get_all_pairs.\n");
		exit(1);
	}
	return aat_tree_inorder_list(counter->tree);
}

/**
 * Free the counter.
 * 
 * @param counter Counter to be freed
 */
void counter_free(Counter* counter) {
	aat_tree_free(counter->tree);
	free(counter);
}

/**
 * Make a new iterator for given Counter.
 * 
 * @param tree Counter to make the iterator upon
 * 
 * @return A new CounterIterator
 * 
 * @note Insertion and deletion of nodes in Counter->tree in between an iterator's lifecycle may not be reflected
 * 		and may cause unexpected errors, especially segmentation fault with deletion
 */
CounterIterator* counter_iterator_make(Counter* counter) {
	AatIterator* tree_iterator_ = aat_iterator_make(counter->tree);
	CounterIterator* counter_iterator = malloc(sizeof(CounterIterator));
	counter_iterator->tree_iterator = tree_iterator_;
	return counter_iterator;
}

/**
 * Check if iterator has traversed through every node in Counter->tree
 * 
 * @param iterator Iterator to check
 * 
 * @return True if iterator has next node, false otherwise
 */
bool counter_iterator_has_next(CounterIterator* iterator) {
	return aat_iterator_has_next(iterator->tree_iterator);
}

/**
 * Get next CounterPair from the iterator
 * 
 * @param iterator Iterator to get next node from
 * 
 * @return The next pair in the iterator, NULL if iterator does not have next pair
 */
CounterPair counter_iterator_next(CounterIterator* iterator) {
	CounterPair pair;
	AatNode* next_node = aat_iterator_next(iterator->tree_iterator);
	strncpy(pair.word, (char*) next_node->key, MAX_WORD_LEN - 1);
	pair.word[MAX_WORD_LEN - 1] = '\0';
	pair.count = *(int*) next_node->value;
	return pair;
}

/**
 * Free given CounterIterator
 * 
 * @param iterator Iterator to free from heap memory
 */
void counter_iterator_free(CounterIterator* iterator) {
	aat_iterator_free(iterator->tree_iterator);
	free(iterator);
}