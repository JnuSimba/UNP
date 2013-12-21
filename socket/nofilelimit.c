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
#include<netdb.h>
#include<sys/time.h>
#include<sys/resource.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)

int main(void)
{
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		ERR_EXIT("getrlimit error");
	printf("%d\n", (int)rl.rlim_max);

	rl.rlim_cur = 2048;
	rl.rlim_max = 2048;
	if (setrlimit(RLIMIT_NOFILE, &rl) < 0)
		ERR_EXIT("setrlimit");

	if (getrlimit(RLIMIT_NOFILE, &rl) <0)
		ERR_EXIT("getrlimit error");

	printf("%d\n", (int)rl.rlim_max);

	return 0;
}

