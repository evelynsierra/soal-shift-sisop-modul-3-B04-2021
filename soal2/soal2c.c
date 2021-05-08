// This program is an example of how to run a command such as
// ps aux | grep root | grep sbin
// using C and Unix.

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int pid;
int fd1[2];
int fd2[2];

void main() {
	pid_t child_id;
  	// create pipe1
  	if (pipe(fd1)==-1) 
	{ 
		exit(1); 
	} 

  	// fork (ps aux)
  	child_id = fork();
  	if (child_id < 0)
    {
        exit(EXIT_FAILURE);
    }
	else if (child_id == 0) {
    	// stdin --> ps --> pipe1
    	// input from stdin (already done)
  		// output to fd1
  		dup2(fd1[1], 1);
  		// close fds
  		close(fd1[0]);
  		close(fd1[1]);
  		// exec
  		char *arg[] = {"ps", "aux", NULL};
  		execv("/bin/ps", arg);
  		// exec didn't work, exit
  		_exit(1);
  	}
  	// parent
	else{
  		// create pipe2
  		if (pipe(fd2) == -1) {
			exit(1); 
  		}
	
		child_id = fork();
  		// fork (grep root)
  		if (child_id < 0)
   	 	{
        	exit(EXIT_FAILURE);
    	}
		else if (child_id == 0) {
    		// fd1 --> grep --> pipe2
    		// input from fd1
  			dup2(fd1[0], 0);
		  	// output to fd2
		  	dup2(fd2[1], 1);
		  	// close fds
		  	close(fd1[0]);
		  	close(fd1[1]);
		  	close(fd2[0]);
		  	close(fd2[1]);
		  	// exec
		  	char *arg[] = {"sort", "-nrk", "3.3",NULL};
		  	execv("/bin/sort", arg);
		  	// exec didn't work, exit
		  	_exit(1);
  		}
  		// parent
		else{
  			// close unused fds
  			close(fd1[0]);
  			close(fd1[1]);

    		// pipe2 --> grep --> stdout
    		// input from fd2
		  	dup2(fd2[0], 0);
		  	// output to stdout (already done)
		  	// close fds
		  	close(fd2[0]);
		  	close(fd2[1]);
		  	// exec
		  	char *arg[] = {"head","-5",NULL}; 
		  	execv("/bin/head",arg);
		  
		  	// exec didn't work, exit
		  	_exit(1);	
		}
	}
}


