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
#include<string.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

#define MSGMAX 8192
struct msgbuf {
	long mtype;
	char mtext[MSGMAX];
};


void echo_ser(int msgid)
{
	struct msgbuf msg;
	memset(&msg, 0, sizeof(msg));
	int nrcv = 0;
	while (1) {

		if ((nrcv = msgrcv(msgid, &msg, MSGMAX, 1, 0)) < 0);
		int pid = *((int*)msg.mtext);
		fputs(msg.mtext+4, stdout);
		msg.mtype = pid;
		msgsnd(msgid, &msg, nrcv, 0);
		memset(&msg, 0, sizeof(msg));

	}
}

int main(int argc, char *argv[])
{
	int msgid;
	msgid = msgget(1234, IPC_CREAT | 0666);
	if (msgid == -1)
		ERR_EXIT("msgget");

	echo_ser(msgid);
	

	return 0;
}


