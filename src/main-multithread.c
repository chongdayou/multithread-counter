#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "../include/counter.h"
#include "../include/aat.h"

Counter* global_counter;
pthread_mutex_t lock;

char* clean_tolower_word(char* word) {
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

void add_to_tree(AatTree* tree, char* key) {
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

void* mythread(void* filename) {
	FILE* file = fopen(filename, "r");

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
			pthread_mutex_lock(&lock);
			add_to_tree(global_counter->tree, cleaned_tolowered_word);
			pthread_mutex_unlock(&lock);
			// it is enough to just pass line into strtok_r() once
			// using NULL here tells strtok_r() to update token_ptr to the next work in line
			word_token = strtok_r(NULL, " ", &token_ptr);
			free(cleaned_tolowered_word);
		}
	}

	free(line);
	fclose(file);

	return NULL;
}

int main(int argc, char* argv[]) {

	global_counter = counter_make();

	pthread_t threads[argc];
	if (pthread_mutex_init(&lock, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(1);
	}

	for (int i=1; i<argc; i++) {
		if (pthread_create(&threads[i], NULL, mythread, argv[i]) != 0) {
			perror("pthread_create");
			exit(1);
		}
		//printf("Created 1 thread, filepath=%s.\n", argv[i]);
	}

	for (int i=1; i<argc; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&lock);

	char* global_counter_string = counter_get_all_pairs(global_counter);
	FILE* output_file = fopen("output-threads.txt", "w");
	fprintf(output_file, "%s\n", global_counter_string);
	fclose(output_file);
	free(global_counter_string);

	counter_free(global_counter);

	return 0;
}