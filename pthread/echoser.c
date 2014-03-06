#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

void echo_srv(int conn)
{
	char recvbuf[1024];
        while (1)
        {
                memset(recvbuf, 0, sizeof(recvbuf));
                int ret = read(conn, recvbuf, sizeof(recvbuf));
		if (ret == 0)
		{
			printf("client close\n");
			break;
		}
		else if (ret == -1)
			ERR_EXIT("read");
                fputs(recvbuf, stdout);
                write(conn, recvbuf, ret);
        }
        close(conn);
}

void* thread_routine(void *arg)
{
	/* 主线程没有调用pthread_join等待线程退出 */
	pthread_detach(pthread_self()); //剥离线程，避免产生僵线程
	/*int conn = (int)arg;*/
	int conn = *((int*)arg);
	free(arg);
	echo_srv(conn);
	printf("exiting thread ...\n");
	return NULL;
}

int main(void)
{
	int listenfd;
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");
	if (listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	int conn;

	while (1)
	{
		if ((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
			ERR_EXIT("accept");

		printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

		pthread_t tid;
//		int ret;
		/*pthread_create(&tid, NULL, thread_routine, (void*)&conn);*/ // race condition问题，竟态问题
		int *p = malloc(sizeof(int));
		*p = conn;
		pthread_create(&tid, NULL, thread_routine, p);
/*
		if ((ret = pthread_create(&tid, NULL, thread_routine, (void*)conn)) != 0) //64位系统时指针不是4个字节，不可移植
		{
			fprintf(stderr, "pthread_create:%s\n", strerror(ret));
			exit(EXIT_FAILURE);
		}
*/
	}
	
	return 0;
}

