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

struct packet {
	int len;
	char buf[1024];
};

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

	struct packet sendbuf;
	struct packet recvbuf;
	memset(&sendbuf, 0, sizeof(sendbuf));
	memset(&recvbuf, 0, sizeof(recvbuf));
/*
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
*/
	int n;
	while (fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL) {
		
		n = strlen(sendbuf.buf);
		sendbuf.len = htonl(n);
	/*
		writen(sock, sendbuf, sizeof(sendbuf)); //发送定长包，不会发生粘包问题，但这样加重了网络负担
		readn(sock, recvbuf, sizeof(recvbuf)); 
	*/
	// 也就是因为结构体没有填充字符，才能认为发出去就是4+n个字节流
	// 最好是将需要发送的数据填充到char 数组发出，就不存在结构体对齐问题
		writen(sock, &sendbuf, 4+n); //自己设定包之间的界定为4字节，分两次读取数据，避免粘包问题
									// 且不会发送固定长度的包，不会加重网络负担
		int ret = readn(sock, &recvbuf.len, 4);
		if (ret == -1)
			ERR_EXIT("read error");
		else if (ret < 4) { //服务器关闭 
			printf("client close\n");
			break;
		}
		
		n = ntohl(recvbuf.len);
		ret = readn(sock, recvbuf.buf, n);
		if (ret == -1)
			ERR_EXIT("read error");
		if (ret < n) { //服务器关闭 
			printf("server close\n");
			break;
		}

		fputs(recvbuf.buf, stdout);
		
		memset(&sendbuf, 0, sizeof(sendbuf));
		memset(&recvbuf, 0, sizeof(recvbuf));
		

	}

	close(sock);
	
	return 0;
}

