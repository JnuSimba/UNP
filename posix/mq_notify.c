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
#include<string.h>
#include<signal.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

typedef struct stu
{
	char name[32];
	int age;
} STU;

size_t size;

mqd_t mqid;

struct sigevent sigev;

void handle(int sig)
{
	mq_notify(mqid, &sigev);
	STU stu;
	unsigned int prio;
	mq_receive(mqid, (char*)&stu, size, &prio);
	printf("name=%s, age=%d, prio=%d\n", stu.name, stu.age, prio);
	
}


int main(int argc, char *argv[])
{
	mqid = mq_open("/abc", O_RDONLY);
	if (mqid == (mqd_t)-1)
		ERR_EXIT("mq_open");
	printf("mq_open succ\n");

	struct mq_attr attr;
	mq_getattr(mqid, &attr);
	size = attr.mq_msgsize;

	signal(SIGUSR1, handle);
	
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;

	mq_notify(mqid, &sigev);

	for (; ;)
		pause();

	mq_close(mqid);

	return 0;
}


