#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	pid_t childPid = fork();
	int status;
	if(childPid == -1) //Failed to fork
	{
		perror("fork error");
		exit(EXIT_FAILURE);
	}
	else if(childPid > 0) //parent
	{
		printf("I am the parent! self PID:%d\tMy child's PID = %d\n", getpid(), childPid);
		waitpid(childPid, &status, 0);
		printf("The child process exited with status %d", status);
	}
	else //child
	{
		sleep(3);
		printf("I am the child! self PID:%d\tMy parent's PID = %d\n", getpid(), getppid());

		//system("./an_ch2_1b");
		char *argv[]={"./an_ch2_1b",NULL, NULL};
		char *envp[]={0,NULL};
		execve("./an_ch2_1b", argv, envp);
	}
}
