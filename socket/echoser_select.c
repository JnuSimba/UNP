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
#include<signal.h>
#include<sys/wait.h>
#include "read_write.h"

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)


int main(void)
{
	
	signal(SIGPIPE, SIG_IGN);
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
	int i;
	int client[FD_SETSIZE];
	int maxi = 0; // client数组中最大不空闲位置的下标
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	int nready;
	int maxfd = listenfd;
	fd_set rset;
	fd_set allset;
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	while (1) {
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready == -1) {
			if (errno == EINTR)
				continue;
			ERR_EXIT("select error");
		}

		if (nready == 0)
			continue;

		if (FD_ISSET(listenfd, &rset)) {
		
			conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);  //accept不再阻塞
			if (conn == -1)
				ERR_EXIT("accept error");
			
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i] < 0) {
					client[i] = conn;
					if (i > maxi)
						maxi = i;
				 	break;
				} 
			}
			
			if (i == FD_SETSIZE) {
				fprintf(stderr, "too many clients\n");
				exit(EXIT_FAILURE);
			}

			printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
				ntohs(peeraddr.sin_port));

			FD_SET(conn, &allset);
			if (conn > maxfd)
				maxfd = conn;

			if (--nready <= 0)
				continue;
		}

		for (i = 0; i <= maxi; i++) {
			conn = client[i];
			if (conn == -1)
				continue;

			if (FD_ISSET(conn, &rset)) {
				
				char recvbuf[1024] = {0};
				int ret = readline(conn, recvbuf, 1024);
				if (ret == -1)
					ERR_EXIT("readline error");
				else if (ret  == 0) { //客户端关闭 
					printf("client close \n");
					FD_CLR(conn, &allset);
				 	client[i] = -1;
					close(conn);
				}
		
				fputs(recvbuf, stdout);
				writen(conn, recvbuf, strlen(recvbuf));
				
				if (--nready <= 0)
				 	break; 
			}
		}


	}
		
	return 0;
}

/* select所能承受的最大并发数受
 * 1.一个进程所能打开的最大文件描述符数，可以通过ulimit -n来调整
 *   但一个系统所能打开的最大数也是有限的，跟内存有关，可以通过cat /proc/sys/fs/file-max 查看
 * 2.FD_SETSIZE(fd_set)的限制，这个需要重新编译内核
 */
