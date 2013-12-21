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
	msgid = msgget(1234, 0);
	if (msgid == -1)
		ERR_EXIT("msgget");
	printf("msgget success\n");
	printf("msgid=%d\n", msgid);

	struct msqid_ds buf;
	msgctl(msgid, IPC_STAT, &buf);
	printf("permission : %o\n", buf.msg_perm.mode);

	sscanf("600", "%o", (unsigned int*)&buf.msg_perm.mode);
	msgctl(msgid, IPC_SET, &buf);

	return 0;
}


