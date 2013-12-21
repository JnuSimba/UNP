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

struct msgbuf {
	long mtype;
	char mtext[1];
};

#define MSGMAX 8192

int main(int argc, char *argv[])
{

	int flag = 0;
	int type = 0;
	int opt;
	while (1) {
		opt = getopt(argc, argv, "nt:");
		if (opt == '?')
			ERR_EXIT("getopt");
		if (opt == -1)
			break;
		switch (opt) {
			case 'n' : // -n 表示IPC_NOWAIT
				flag |= IPC_NOWAIT;
				break;
			case 't': // -t
				type = atoi(optarg);
				break;
		}
	}



	int msgid;
	msgid = msgget(1234, 0);
	if (msgid == -1)
		ERR_EXIT("msgget");
	
	struct msgbuf* ptr;
	ptr = (struct msgbuf*)malloc(sizeof(long) + MSGMAX);
	ptr->mtype = type;
	int nrcv = 0;
	if ((nrcv = msgrcv(msgid, ptr, MSGMAX, type, flag)) < 0)
		ERR_EXIT("msgsnd");

	printf("read %d bytes type=%ld\n", nrcv, ptr->mtype);

	return 0;
}


