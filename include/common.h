#ifndef COMMON_H
#define COMMON_H

#define MAX_MSG_LEN 1024

typedef struct _proc_message {
	long type;
	char text[MAX_MSG_LEN];
} _proc_message;

typedef struct __proc_message {
	long type;
	long count;
	char text[];
} __proc_message;

typedef enum _message_type {
	// CON_KEY_MSG, END_KEY_MSG, EXIT_MSG
	STR_MSG = 1, EXIT_MSG
} _message_type;

#endif// COMMON_H