#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid = fork();
	
	if(pid == 0)
	{
		puts("Hi, I'm your son");
	}
	else
	{
		printf("Child Process ID: %d \n", pid);
		sleep(40);
	}
	
	if(pid == 0)
	{
		puts("End child process");
		
	}
	else
	{
		puts("End parent process");
	}
	
	return 0;
}