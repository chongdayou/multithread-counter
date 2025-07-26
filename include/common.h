#ifndef COMMON_H
#define COMMON_H

#define MAX_MSG_LEN 1024
#define MAX_WORD_LEN 128

typedef struct _word_count_message {
	long type;
	int count;
	char text[MAX_WORD_LEN];
} _word_count_message;

typedef enum _message_type {
	// CON_KEY_MSG, END_KEY_MSG, EXIT_MSG
	END_PAIR_MSG = 1, CON_PAIR_MSG, EXIT_MSG
} _message_type;

#endif// COMMON_H