#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock, i;
	int str_len;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	int count = 0;
	
	if(argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	{
		error_handling("socket()error");
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
	{
		error_handling("connect()error");
	}
	else
	{
		puts("Connected.......");
	}
	
	
	fputs("输入总数: (<100)", stdout);
	fgets(message, BUF_SIZE, stdin);
	write(sock, message, strlen(message));
	str_len = read(sock, message, BUF_SIZE-1);
	message[str_len] = 0;
	printf("总数为：: %s", message);
	
	
	count = atoi(message);
	for(i = 0; i < count; i++)
	{
		fputs("输入数字: ", stdout);
		fgets(message, BUF_SIZE, stdin);
		write(sock, message, strlen(message));
		str_len = read(sock, message, BUF_SIZE-1);
		message[str_len] = 0;
		printf("第%d个数字为：: %s", i+1, message);
	}
	
	fputs("输入操作符: ", stdout);
	fgets(message, BUF_SIZE, stdin);
	write(sock, message, strlen(message));
	str_len = read(sock, message, BUF_SIZE-1);
	message[str_len] = 0;
	printf("答案为：: %s", message);
	


	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}