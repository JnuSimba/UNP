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
#include<mqueue.h>
#include<fcntl.h>
#include<sys/stat.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main(void)
{
	mqd_t mqid;
	mqid = mq_open("/abc", O_RDONLY);
	if (mqid == (mqd_t)-1)
		ERR_EXIT("mq_open");
	printf("mq_open succ\n");
	
	struct mq_attr attr;
	mq_getattr(mqid, &attr);
	printf("max #msg=%ld max #bytes/msg=%ld #currently on queue=%ld\n",
			attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
	mq_close(mqid);
	return 0;
}


