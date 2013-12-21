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
	fd_set rset;
	FD_ZERO(&rset);

	int nready;
	int maxfd;
	int fd_stdin = fileno(stdin); //标准输入可能被重定向
	if (fd_stdin > sock)
		maxfd = fd_stdin;
	else
		maxfd = sock;

	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};

	while (1) {

		FD_SET(fd_stdin, &rset);
		FD_SET(sock, &rset);
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL); //select返回表示检测到可读事件
		if (nready == -1)
			ERR_EXIT("select error");

		if (nready == 0)
			continue;

		if (FD_ISSET(sock, &rset)) {

			int ret = readline(sock, recvbuf, sizeof(recvbuf)); //按行读取
			if (ret == -1)
				ERR_EXIT("read error");
			else if (ret  == 0) { //服务器关闭 
				printf("server close\n");
				break;
			}
		
			fputs(recvbuf, stdout);
			memset(recvbuf, 0, sizeof(recvbuf));
		}

		if (FD_ISSET(fd_stdin, &rset)) {

			if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL) 
				break;

			writen(sock, sendbuf, strlen(sendbuf)); 
			memset(sendbuf, 0, sizeof(sendbuf));
		}
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

