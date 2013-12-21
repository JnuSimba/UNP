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


int main(void)
{
	int sockfds[2];

	if (socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds) < 0)
		ERR_EXIT("sockpair");

	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fork");

	if (pid > 0) {
		int val = 0;
		close(sockfds[1]);
		while (1) {
			
			++val;
			printf(" sending data: %d\n", val);
			write(sockfds[0], &val, sizeof(val));
			read(sockfds[0], &val, sizeof(val));
			printf("recv data : %d\n", val);
			sleep(1);
		}

	}

	else if (pid == 0) {

		int val;
		close(sockfds[0]);
		while (1) {

			read(sockfds[1], &val, sizeof(val));
			++val;
			write(sockfds[1], &val, sizeof(val));
		}
	}

	return 0;
}


