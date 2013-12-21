/*************************************************************************
	> File Name: basic.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Tue 12 Mar 2013 06:54:20 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main(void)
{
	int msgid;
	msgid = msgget(1234, 0666 | IPC_CREAT);
	if (msgid == -1)
		ERR_EXIT("msgget");
	printf("msgget success\n");
	msgid = msgget(1234, 0);
	printf("msgid=%d\n", msgid);

	return 0;
}


