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

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)

void do_service(int);

int main(void)
{
	int listenfd; //被动套接字(文件描述符），即只可以accept, 监听套接字
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
//	listenfd = socket(AF_INET, SOCK_STREAM, 0)	
		ERR_EXIT("socket error");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	/* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
	/* inet_aton("127.0.0.1", &servaddr.sin_addr); */
	
	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		ERR_EXIT("setsockopt error");

	if (bind(listenfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
		ERR_EXIT("bind error");

	if (listen(listenfd, SOMAXCONN) < 0) //listen应在socket和bind之后，而在accept之前
		ERR_EXIT("listen error");
	
	struct sockaddr_in peeraddr; //传出参数
	socklen_t peerlen = sizeof(peeraddr); //传入传出参数，必须有初始值
	int conn; // 已连接套接字(变为主动套接字，即可以主动connect)

	pid_t pid;

	while (1) {
		if ((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0) //3次握手完成的序列
			ERR_EXIT("accept error");
		printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
				ntohs(peeraddr.sin_port));
		
		pid = fork();
		if (pid == -1)
			ERR_EXIT("fork error");
		if (pid == 0) {
			// 子进程
			close(listenfd);
			do_service(conn);
			exit(EXIT_SUCCESS);
		}
		else
			close(conn); //父进程
	}
	
	return 0;
}

void do_service(int conn)
{
	char recvbuf[1024];
	while (1) {
		memset(recvbuf, 0, sizeof(recvbuf));
		int ret = read(conn, recvbuf, sizeof(recvbuf));
		if (ret == 0) { //客户端关闭了
			printf("client close\n");
			break;
		}
		else if (ret == -1)
			ERR_EXIT("read error");
		fputs(recvbuf, stdout);
		write(conn, recvbuf, ret);
	}
}

