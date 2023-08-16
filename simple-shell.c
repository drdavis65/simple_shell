#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


void tokenize(char *buf, char **args);
void tokenize_pipe(char *buf, char** pCmds);


#define MAX_LINE		80

int main(void)
{
	char *args[MAX_LINE/2 + 1];
	int should_run = 1;
	char history[MAX_LINE];
	static char buf[MAX_LINE];
	int has_pipe;


	while (should_run)
	{
		has_pipe = 0;	// reset pipe flag

		printf("osh>");
		fflush(stdout);

		fgets(buf, sizeof(buf), stdin); // get user input
						//
		if((strlen(buf)-1)==0)
		{
			continue; // if user input is empty prompt again
		}
		else
		{
			buf[strlen(buf)-1]=0; 	// get rid of newline character
		}


		if(strchr(buf, '|'))
			has_pipe = 1;
			// check for pipe
		


		if(has_pipe)
		{
			int fd[2];
			char *pCmds[10];
			tokenize_pipe(buf, pCmds);
			if(pipe(fd) == -1)
			{
				perror("Pipe failed\n");
			}
			if(fork() == 0)
			{
				close(STDOUT_FILENO);
				dup(fd[1]);
				close(fd[0]);
				close(fd[1]);

				tokenize(pCmds[0], args);
				execvp(args[0], args);
				fprintf(stderr, "execvp of %s has failed\n", args[0]);
				exit(1);
			}
			if(fork() == 0) 
			{
				close(STDIN_FILENO);
				dup(fd[0]);
				close(fd[1]);
				close(fd[0]);

				tokenize(pCmds[1], args);
				execvp(args[0], args);
				fprintf(stderr, "execvp of %s has failed\n", args[0]);
				exit(1);
			}
			close(fd[0]);
			close(fd[1]);
			wait(0);
			wait(0);
		}
		else {
			tokenize(buf, args);	
			pid_t pid;

			pid = fork();
			if(pid == 0) {
				if(execvp(args[0],args) == -1) {
					printf("Terminated incorrectly");
				}
			}
			else{
				wait(NULL);
			}
		}
		
	}

	return 0;
}

void tokenize(char *buf, char **args) {
	char *token = strtok(buf, " "); // collect arguments by tokenizing with space
	int argc = 0;
	while(token != NULL) {
		args[argc++] = token;
		token = strtok(NULL, " ");
	}
	args[argc] = NULL;
}	

void tokenize_pipe(char *buf, char** pCmds) {
	char *token = strtok(buf, "|");
	int i = 0;
	while(token != NULL) {
		pCmds[i++] = token;
		token = strtok(NULL, "|");
	}
	pCmds[i] = NULL;
}

