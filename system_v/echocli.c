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

void echo_cli(int msgid)
{
	int nrcv;
	int pid = getpid();
	struct msgbuf msg;
	memset(&msg, 0, sizeof(msg));
	msg.mtype = 1;
	*((int*)msg.mtext) = pid;
	while (fgets(msg.mtext+4, MSGMAX, stdin) != NULL) {
	
		if (msgsnd(msgid, &msg, 4 + strlen(msg.mtext+4), IPC_NOWAIT) < 0)
			ERR_EXIT("msgsnd");
		
		memset(msg.mtext+4, 0, MSGMAX-4);
		if ((nrcv = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0)
				ERR_EXIT("msgsnd");
		fputs(msg.mtext+4, stdout);
		memset(msg.mtext+4, 0, MSGMAX-4);

	}
}

int main(int argc, char *argv[])
{

	int msgid;
	msgid = msgget(1234, 0);
	if (msgid == -1)
		ERR_EXIT("msgget");
	
	echo_cli(msgid);

	return 0;
}


