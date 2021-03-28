#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void err_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	int fds[2];
	
	pid_t pid;
	struct sigaction act;
	socklen_t addr_size;
	int str_len, state;
	char buf[BUF_SIZE];
	
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	//信号处理，用于避免僵尸进程
	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	state = sigaction(SIGCHLD, &act, 0);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	//socket结构体初始化	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	//bind
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		err_handling("bind() error");
	}
	
	//listen，监听
	if(listen(serv_sock, 5) == -1)
	{
		err_handling("listen() error");
	}
	
	//创建新的子进程用来读管道数据并将其写入文件
	pipe(fds);
	pid = fork();
	if(pid == 0)
	{
		FILE* fp = fopen("echomsg.txt", "wt");
		char msgbuf[BUF_SIZE];
		int i, len;
		
		for(i = 0; i < 10; i++)
		{
			len = read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);
		}
		fclose(fp);
		return 0;
	}
	
	//处理部分，accept后创建子进程并调用管道进行进程间通讯
	while(1)
	{
		addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
		
		if(clnt_sock == -1)
		{
			continue;
		}
		else
		{
			puts("new clinet connected...");
		}
		pid = fork();
		if(pid == -1)
		{
			close(clnt_sock);
			continue;
		}
		if(pid == 0) //child
		{
			close(serv_sock);
			while((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
			{
				write(clnt_sock, buf, str_len);
				write(fds[1], buf, str_len);
			}
			
			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else
		{
			close(clnt_sock);
		}
	}
	close(serv_sock);
	
	return 0;
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("remove proc id: %d \n", pid);
}

void err_handling(char *message)
{
	fputs(message, stderr);
	putc('\n', stderr);
	exit(1);
}
