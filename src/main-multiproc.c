#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>

#include "../include/counter.h"
#include "../include/strbuffer.h"
#include "../include/common.h"

typedef struct _proc_receiver_struct {
	_word_count_message message;
	int msg_queue_id;
	key_t key;
	Counter* counter;
	StrBuffer* strbuffer;
} _proc_receiver_struct;

void proc_sender(char* filepath) {
	//printf("In proc_sender: execvp on ./build/senderMain with %s\n", filepath);
    //fflush(stdout);
	char* args[] = {"./build/senderMain", filepath, NULL};
	//printf("filepath=%s\n", filepath);
	execvp("./build/senderMain", args);
	perror("execvp failed");
	exit(1);
}

void proc_receiver(_proc_receiver_struct* receiver_struct) {
	while (1) {
		//printf("Waiting to receive from queue=%d...\n", receiver_struct->msg_queue_id);
		if (msgrcv(receiver_struct->msg_queue_id, &receiver_struct->message, sizeof(_word_count_message) - sizeof(long), 0, 0) == -1) {
			perror("msgrcv");
			exit(1);
		}

		//printf("Received message:%s=%d\n", receiver_struct->message.text, receiver_struct->message.count);

		if (receiver_struct->message.type == EXIT_MSG) {
			//printf("break condition met.\n");
			break;
		} else {
			CounterPair pair;
			strncpy(pair.word, receiver_struct->message.text, MAX_WORD_LEN - 1);
			pair.word[MAX_WORD_LEN - 1] = '\0';
			pair.count = receiver_struct->message.count;
			counter_add_pair(receiver_struct->counter, pair);
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
			//printf("About to fork [%d],filepath=%s\n", i, argv[i]);
			proc_sender(argv[i]);
			perror("execvp failed");
			exit(1);
		}
	}

	// in parent
	Counter* global_counter = counter_make();
	StrBuffer* strbuffer = strbuffer_make(INITIAL_CAPACITY);

	for (int i=1; i<argc; i++) {
		_proc_receiver_struct receiver_struct;
		receiver_struct.counter = global_counter;
		receiver_struct.strbuffer = strbuffer;

		if ((receiver_struct.key = ftok(argv[i], 1)) == -1) {
			perror("ftok");
			exit(1);
		}

		if ((receiver_struct.msg_queue_id = msgget(receiver_struct.key, 0666 | IPC_CREAT)) == -1) {
			perror("msgget");
			exit(1);
		}
		//printf("parent working on file=%siteration=%dqueue=%d\n", argv[i], i, receiver_struct.msg_queue_id);
		//printf("Receiving message [%d].\n", i);
		proc_receiver(&receiver_struct);
		wait(NULL);
		if ((msgctl((&receiver_struct)->msg_queue_id, IPC_RMID, NULL)) == -1) {
			perror("msgctl[IPC_RMID]");
			exit(1);
		}
	}

	char* all_pairs = counter_get_all_pairs(global_counter);
	FILE* output_file = fopen("output-procs.txt", "w");
	fprintf(output_file, "%s\n", all_pairs);
	fclose(output_file);
	free(all_pairs);

	// for (int i=1; i<argc; i++) {
	// 	wait(NULL);
	// }

	counter_free(global_counter);
	strbuffer_free(strbuffer);
	//free(receiver_struct);

	return 0;
}



/*

struct message_queue_data {
	queue_id
};

struct message {
	type
	count
	str
};

// code to make the queue here

void send_msg(AatTree* tree, void * raw_key, void * data_) {
	struct messeage_queue_data data = data_;
	struct message msg = {NORMAL_MSG, tree->get_val(node), tree->get_key(node)};
	msgsnd(data->queue_id, msg, MSG_LEN)
}

message_queue_data mqd = {queue_id};
AatTree tree;
aat_tre/e_walk(tree, send_msg, &mqd);

// code to delete the queue here




*/