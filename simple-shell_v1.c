#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE		80

int main(void)
{
	char *args[MAX_LINE/2 + 1];
	int should_run = 1;
	char *history[20];
	static char buf[100];

	while (should_run)
	{
		printf("osh>");
		fflush(stdout);

		/**
		 * after reading user input,
		 * 1 fork a child process
		 * 2 child process will invoke execvp()
		 * 3 if command included & parent will invoke wait()
		 */
		fgets(buf, sizeof(buf), stdin);
		//printf("%s", buf);
		//printf("%lu", strlen(buf));
		if((strlen(buf)-1)==0){
			continue;
		}
		buf[strlen(buf)-1]=0;
		char *token = strtok(buf, " ");
		//char *cmd = token;

		int argc = 0;
		while(token != NULL) {
			args[argc++] = token;
			//printf("%s\n", token);
			token = strtok(NULL, " ");
		}
		args[argc] = NULL;
		
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

	return 0;
}
