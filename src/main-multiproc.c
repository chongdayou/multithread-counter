#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "../include/counter.h"
#include "../include/strbuffer.h"

#define MAX_MSG_LEN 1024

typedef struct _proc_message {
	long type;
	char text[MAX_MSG_LEN];
} _proc_message;

typedef struct _proc_receiver_struct {
	_proc_message message;
	int msg_queue_id;
	key_t key;
	Counter* counter;
	StrBuffer* strbuffer;
} _proc_receiver_struct;

void proc_sender(char* filepath) {
	printf("In proc_sender: execvp on ./build/senderMain with %s\n", filepath);
    fflush(stdout);
	char* args[] = {"./build/senderMain", filepath, NULL};
	printf("filepath=%s\n", filepath);
	execvp("./build/senderMain", args);
	perror("execvp failed");
	exit(1);
}

void proc_receiver(_proc_receiver_struct* receiver_struct) {
	while (1) {
		if (msgrcv(receiver_struct->msg_queue_id, &receiver_struct->message, MAX_MSG_LEN, 0, 0) == -1) {
			perror("msgrcv");
			exit(1);
		}

		printf("Received message:%s\n", receiver_struct->message.text);

		if (receiver_struct->message.text[0] == '\0') {
			break;
		}
	}
}

int main(int argc, char* argv[]) {

	for (int i=1; i<argc; i++) {
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork failed");
			exit(1);
		} else if (pid == 0) {
			printf("About to fork [%d]\n", i);
			proc_sender(argv[i]);
			perror("execvp failed");
			exit(1);
		}
	}

	// in parent
	Counter* global_counter = counter_make();
	StrBuffer* strbuffer = strbuffer_make(INITIAL_CAPACITY);
	_proc_receiver_struct* receiver_struct = malloc(sizeof(_proc_receiver_struct));
	receiver_struct->counter = global_counter;
	receiver_struct->strbuffer = strbuffer;
	if ((receiver_struct->key = ftok("./src/main-sender.c", 1)) == -1) {
		perror("ftok");
		exit(1);
	}

	if ((receiver_struct->msg_queue_id = msgget(receiver_struct->key, 0400)) == -1) {
		perror("msgget");
		exit(1);
	}
	for (int i=1; i<argc; i++) {
		printf("Receiving message [%d].\n", i);
		proc_receiver(receiver_struct);
		wait(NULL);
	}
	if ((msgctl(receiver_struct->msg_queue_id, IPC_RMID, NULL)) == -1) {
		perror("msgctl[IPC_RMID]");
		exit(1);
	}

	counter_free(global_counter);
	strbuffer_free(strbuffer);
	free(receiver_struct);

	return 0;
}