#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {

	pid_t fork_pid = fork();
	pid_t this_pid = getpid();
	pid_t sec_fork = fork();
	printf("parent=%d,first=%d,second=%d\n", this_pid, fork_pid, sec_fork);

	return 0;
}