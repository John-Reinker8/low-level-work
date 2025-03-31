#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
// added inclusion of errno for error handling
#include <errno.h>

int main(int argc, char *argv[]) {
	// argc is length of the argv array, which contains pointers to each program fed to this program
	// however, indexing starts at 1 and goes to argc - 1. it does not start at 0

	if (argc <= 1) {	// handles when we are given zero programs to run
		errno = EINVAL;	// set errno per spec
		perror("Zero arguments given");
		exit(errno);	// exit
	}

	else if (argc == 2) {	// handles when we are given 1 program to run, so we can skip all the pipe nonsense
		pid_t pid = fork();	// create one child process to take care of our own program arguement
							// when fork is called, two processes continue onward
								// the parent process gets a pid that is some positive number representing the child
								// the child gets 0 as the pid value

		if (pid == 0) {     // case where the process is the child, so run the one program given
			execlp(argv[1], argv[1], NULL);		// child calls execlp
			perror("Call to execlp failed");	// these next two lines should hopefully not run
			exit(errno);
		}
		else if (pid > 0) {	// case where the process is the parent process, so make it wait for child process to finish
			int status;     // create status object to be passed to waitpid
			waitpid(pid, &status, 0);	// calls waitpid with the child process pid number, a pointer to the status object, and 0 options
			if (WEXITSTATUS(status) != 0) {	// added to help pass bogus test case, need parent to handle child errors
				perror("Parent caught a child not executing properly in single program case");
				exit(WEXITSTATUS(status));
			}
		}
		else {	     		// case where fork fails
			perror("Fork failed in single program case");
			exit(errno);
		}
	}
	// alright so code from here on handles when there are >= 2 programs (so the actual piping comes into play)
	else {
		int prgs = argc - 1;	// figure out how many more programs we have to run after the first
		pid_t pids[prgs]; 		// an array to store child pids
		int pipes[prgs - 1][2]; // figure out how many pipes we need, which is gonna be prgrms - 1 (since we always need 1 less "divider")
									// this is a 2d array. the main array is the number of "pipe groups" we have
									// the 2nd array, which is size 2, is size 2 because each pipe group has two members:
									// the read end (x), and the write end (y) in the example x | y
		
		for (int i = 0; i < prgs - 1; i++) {	// creates the actual pipe groups and fills them into our pipes 2d array
			if (pipe(pipes[i]) == -1) {			// call to "pipe" fills the given array, which is a size 2 array at the i index of pipes
												// with the read end and the write end
				perror("Error creating pipe");
				exit(errno);
			}
		}

		for (int i = 0; i < prgs; i++) {	// here we start forking for each child process needed to run each program
			pids[i] = fork();	// set the pid in the pids array

			// visualization help...
			// pipe(i) connects program(i) to program(i + 1)
			// so, program(i) reads from pipe(i - 1) and writes to pipe(i) (not factoring in edge cases)

			if (pids[i] == 0) {     // case where the process is the child
				if (i > 0) {	// case where child is NOT the first process
					if (dup2(pipes[i-1][0], 0) == -1) {	// connects the input of this child to the prev pipe's read end (pipes[i-1][0])
						perror("Dup2 for input failed");
						exit(errno);
					}
				}
				if (i != prgs - 1) {	// case where child is NOT the final process
					if (dup2(pipes[i][1], 1) == -1) {	// connects the output of this child to cur pipe's write end (pipes[i][1])
						perror("Dup2 for output failed");
						exit(errno);
					}
				}

				for (int k = 0; k < prgs - 1; k++) {	// clean up by closing all the pipes in the child
					close(pipes[k][0]);
					close(pipes[k][1]);
				}

				execlp(argv[i+1], argv[i+1], NULL);	// actually run the program
				perror("Child failed to run program in multi program case"); // these next two lines should hopefully not run
				exit(errno);
			}
			else if (pids[i] > 0) {	// case where the process is the parent process, so just continue loop
				continue;
			}
			else {	     		// case where fork fails
				perror("Fork failed somewhere in multi program case");
				exit(errno);
			}
		}

		for (int i = 0; i < prgs - 1; i++) {	// parent closes all its pipes
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		for (int i = 0; i < prgs; i++) {	// make parent wait for all children
			int status;
			waitpid(pids[i], &status, 0);
			if (WEXITSTATUS(status) != 0) {	// added to help pass bogus test case, need parent to handle child errors
				perror("Parent caught a child not executing properly in multi program case");
				exit(WEXITSTATUS(status));
			}
		}
	}
	return 0;
}

