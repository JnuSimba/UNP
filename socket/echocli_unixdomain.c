/*************************************************************************
	> File Name: echoser_udp.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/un.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0);

void echo_cli(int conn)
{
	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {

		
		write(conn, sendbuf, strlen(sendbuf));
		read(conn, recvbuf, sizeof(recvbuf));
		fputs(recvbuf, stdout);
		memset(recvbuf, 0, sizeof(recvbuf));
		memset(sendbuf, 0, sizeof(sendbuf));
	}

	close(conn);
}
		

int main(void)
{
	int sock;
	if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
			ERR_EXIT("socket error");

	struct sockaddr_un servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, "/tmp/test socket");

	if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("connect error");

	echo_cli(sock);

	return 0;
}



