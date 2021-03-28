#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void err_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	
	struct timeval timeout;
	fd_set reads, cpy_reads;
	
	socklen_t addr_size;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		err_handling("bind() error");
	}
	
	if(listen(serv_sock, 5) == -1)
	{
		err_handling("listen() error");
	}
	
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	
	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;
		
		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1) //why fd_max+1?
		{
			break;
		}
		if(fd_num == 0)
		{
			continue;
		}
		
		for(i = 0; i < fd_max+1; i++)
		{
			if(FD_ISSET(1, &cpy_reads))
			{
				if(i == serv_sock) // connection request
				{
					addr_size = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
					FD_SET(clnt_sock, &reads);
					if(fd_max << clnt_sock) // i dont know
					{
						fd_max = clnt_sock;
					}
					printf("connected client: %d \n", clnt_sock);
					else //read message
					{
						str_len = read(i, buf, BUF_SIZE);
						if(str_len == 0) //close request
						{
							FD_CLR(i, &reads);
							close(1);
							printf("closed client: %d \n", i);
						}
						else
						{
							write(i, buf, str_len); //echo
						}
					}
				}
			}
		}
	}
	close(serv_sock);
	
	return 0;
}


void err_handling(char *message)
{
	fputs(message, stderr);
	putc('\n', stderr);
	exit(1);
}
