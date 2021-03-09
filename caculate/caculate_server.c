#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

enum Operator
{
	PLUS = 0x00,
	DEC  = 0x01,
	MUL  = 0x02,
	DIV  = 0x03
};
	
int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	char message[BUF_SIZE], temp[100];
	int operator, count;
	int number[100];
	int str_len, i, pre_len, j, ans, k;
	
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
	{
		error_handling("socket()error");
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
	{
		error_handling("bind()error");
	}
	
	if(listen(serv_sock, 5) == -1)
	{
		error_handling("listen()error");
	}
	
	clnt_addr_size = sizeof(clnt_addr);
	
	for( i = 0; i < 5; i++)
	{
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
		{
			error_handling("accept()error");
		}
		else
		{
			printf("Connect client %d \n", i+1);
		}
		
		if((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
		{
			count = atoi(message);
			pre_len = str_len;
			write(clnt_sock, message, str_len);
		}
		else
		{
			error_handling("Please input count (<100)\n");
		}
		
		for(j = 0; j < count; j++)
		{
			if((str_len = read(clnt_sock, message+pre_len, BUF_SIZE)) != 0)
			{
				number[j] = atoi(message + pre_len);
				write(clnt_sock, message+ pre_len, str_len);
				pre_len += str_len;
			}
			else
			{
				error_handling("Please input num(<255) \n");
			}
		}
		
		if((str_len = read(clnt_sock, message+pre_len, BUF_SIZE)) != 0)
		{
			ans = 0;
			switch(message[pre_len])
			{
				case('+'):
				{
					operator = PLUS;
					
					for(k = 0; k < count; k++)
					{
						ans += number[k];
					}
					break;
				}
				case('-'):
				{
					operator = DEC;
					
					for(k = 0; k < count; k++)
					{
						ans -= number[k];
					}
					break;
				}
				case('*'):
				{
					operator = MUL;
					
					for(k = 0; k < count; k++)
					{
						ans *= number[k];
					}
					break;
				}
				case('/'):
				{
					operator = DIV;
					
					for(k = 0; k < count; k++)
					{
						ans /= number[k];
					}
					break;
				}
				default:
				{
					error_handling("Please input operator (+ - * /) \n");
				}
			}
			write(clnt_sock, message+ pre_len, str_len);
			pre_len = str_len+1;
		}
		else
		{
			error_handling("Please input operator (+ - * /)\n");
		}
		
		sprintf(temp,"%d\n",ans);
		write(clnt_sock, temp, sizeof(ans));
		
		close(clnt_sock);
	}
	
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}