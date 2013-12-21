/*************************************************************************
	> File Name: echoser.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Fri 01 Mar 2013 06:15:27 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include "read_write.h"

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)

void do_echocli(int sock)
{

	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
		
	
		writen(sock, sendbuf, strlen(sendbuf)); 

		int ret = readline(sock, recvbuf, sizeof(recvbuf)); //按行读取
		if (ret == -1)
			ERR_EXIT("read error");
		else if (ret  == 0) { //服务器关闭
			printf("server close\n");
			break;
		}
		
		fputs(recvbuf, stdout);
		
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));
		
	}

	close(sock);
}

int main(void)
{
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
//	listenfd = socket(AF_INET, SOCK_STREAM, 0)	
		ERR_EXIT("socket error");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	/* inet_aton("127.0.0.1", &servaddr.sin_addr); */
	
	if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("connect error");
	
	struct sockaddr_in localaddr;
	socklen_t addrlen = sizeof(localaddr);
	if (getsockname(sock, (struct sockaddr*)&localaddr, &addrlen) < 0)
		ERR_EXIT("getsockname error");
	/* getpeername()获取对等方的地址 */
	printf("local ip=%s port=%d\n", inet_ntoa(localaddr.sin_addr),
				ntohs(localaddr.sin_port));


	do_echocli(sock);

	return 0;
}

