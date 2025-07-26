#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <unistd.h>

#include "../include/counter.h"
#include "../include/aat.h"
#include "../include/common.h"

// TODO: add mechanism to handle the case when message length exceeds _word_count_message.text capacity

int main(int argc, char* argv[]) {
	//printf("senderMain: started with file %s\n", argv[1]);
	//fflush(stdout);
	_word_count_message message;
	int msg_queue_id;
	key_t key;

	if ((key = ftok(argv[1], 1)) == -1) {
		perror("ftok");
		exit(1);
	}

	if ((msg_queue_id = msgget(key, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		perror("fopen");
		exit(1);
	}

	Counter* counter = counter_make();
	counter_into_tree(counter, file);
	fclose(file);

	CounterIterator* counter_iterator = counter_iterator_make(counter);

	CounterPair pair;
	//printf("here\n");
	while (counter_iterator_has_next(counter_iterator)) {
		//printf("Sender queue=[%d] is between has_next and next.\n", msg_queue_id);
		pair = counter_iterator_next(counter_iterator);
		//printf("Sender queue=[%d] got next pair.\n", msg_queue_id);
		message.count = pair.count;
		strcpy(message.text, pair.word);
		message.type = END_PAIR_MSG;
		//printf("Sender sending: word=%s,count=%d,hasNext=%s,queue=%d.\n", message.text, message.count, counter_iterator_has_next(counter_iterator) ? "t" : "f", msg_queue_id);
		if (msgsnd(msg_queue_id, &message, sizeof(_word_count_message) - sizeof(long), 0) == -1) {
			perror("msgsnd");
			exit(1);
		}
	}
	//printf("Sender queue = [%d] finished sending.\n", msg_queue_id);

	// empty message to indicate end of message
	message.type = EXIT_MSG;
	message.count = 0;
	message.text[0] = '\0';
	//printf("Sender sent EXIT_MSG to queue=%d\n", msg_queue_id);
	if (msgsnd(msg_queue_id, &message, MAX_WORD_LEN, 0) == -1) {
		perror("msgsnd");
		exit(1);
	}

	counter_iterator_free(counter_iterator);
	counter_free(counter);

	//printf("senderMain: finished.\n");
	//fflush(stdout);

	return 0;
/*
	char* word_pairs = counter_get_all_pairs(counter);
	printf("Calling counter_get_all_pairs\n");
	if (!word_pairs) {
		fprintf(stderr, "word_pairs is NULL — counter_get_all_pairs failed\n");
		exit(1);
	}
	int msg_index = 0;
	size_t msg_size = strlen(word_pairs) + 1;
	printf("About to enter message loop.\n");
	int retry_count = 0;
	while (msg_index < msg_size) {
		message.type = STR_MSG;
		int chunk_len = msg_size - msg_index >= MAX_MSG_LEN - 1 ? MAX_MSG_LEN - 1 : msg_size - msg_index;
		strncpy(message.text, word_pairs + msg_index, chunk_len);
		printf("About to send message.filepath=%smsg_index=%d\n", argv[1], msg_index);
		msg_index += MAX_MSG_LEN - 1;
		while (msgsnd(msg_queue_id, &message, strlen(message.text) + 1, 0) == -1) {
			if (errno == EAGAIN) {
				if (++retry_count > 10000) {
					fprintf(stderr, "msgsnd: too many retries\n");
					exit(1);
				}
				usleep(1000);  // 1ms
			} else {
				perror("msgsnd");
				exit(1);
			}
		}
	}

	// empty message to indicate end of message
	message.type = EXIT_MSG;
	message.text[0] = '\0';
	if (msgsnd(msg_queue_id, &message, MAX_MSG_LEN, 0) == -1) {
		perror("msgsnd");
		exit(1);
	}

	free(word_pairs);
	counter_free(counter);
*/

	//printf("senderMain: finished.\n");
	//fflush(stdout);

	return 0;
}