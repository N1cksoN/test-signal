// This work is done by Shevchenko Mykyta
// ID: 295461

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>   //On windows it is not necessary to use 'sys/'
#include <stdlib.h>
#include <unistd.h>


#define NUM_CHILD 5
#define WITH_SIGNALS

#ifdef WITH_SIGNALS
	char interrupt_flag = 0;

	void keyboardInterrupt() {
		printf(" -> parent [%d]: The keyboard interrupt has just received.\n", getpid());
		interrupt_flag = 1;
	}
	
	// terminating the process 
	void terminateChild() {
		printf("child [%d]: Termination of the process.", getpid());
	}
#endif

int main() {
	pid_t child_pid = 1;		// child process PID
	pid_t parent_pid = 1;		// parent process PID
	int count = 0;				//counter
	
	printf("Our parent[%d] \n", getpid());
	for (int i = 0; i < NUM_CHILD; ++i) {
		parent_pid = getpid();

		// All signals force ignore
		#ifdef WITH_SIGNALS
			for(int j = 0; j < NSIG; ++j) { // NSIG is the total number of signals
				sigignore(j);
			}
			signal (SIGCHLD, SIG_DFL);	
			signal (SIGINT, keyboardInterrupt); //signal handler for keyboard interrupt 
		#endif

		if (!(child_pid = fork())) {
			// ignoring the keyboard interrupt signal
			#ifdef WITH_SIGNALS
				signal (SIGINT, terminateChild);
				signal (SIGINT, SIG_DFL);
			#endif
			printf("parent's id[%d] -- created a child[%d].\n", getppid(), getpid()); // Printing child's id
			sleep(10);
			printf("child [%d]: I've completed execution.\n", getpid());
			exit(0);
			}
		else if (child_pid == -1) {
			printf("parent [%d]: Couldn't create new child.\n", getpid());
			kill(-1, SIGTERM);
			exit(1);
		}
		sleep(1);

		// checking for the mark, which may be set by the keyboard interrupt
		#ifdef WITH_SIGNALS
			if (interrupt_flag) {
				printf("parent [%d]: Interrupt of the creation process!\n", getpid());
				kill(-2, SIGTERM);
				break;
			}
		#endif
	}
	
	while(1) {
		int s;
		int w = wait(&s);
		if(w == -1)
			break;
		else{
			printf("child [%d]: I've finished.\n", w);
			count++;
		}
	}
	printf("\nSuccess: %d processes were terminated.\n", count);

	// restore old handlers
	#ifdef WITH_SIGNALS
		for(int j = 0; j < NSIG; j++) {
			signal(j, SIG_DFL);
		}
	#endif
	return 0;
}
	
