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

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <prio>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	mqd_t mqid;
	mqid = mq_open("/abc", O_WRONLY);
	if (mqid == (mqd_t)-1)
		ERR_EXIT("mq_open");
	printf("mq_open succ\n");
	
	STU stu;
	strcpy(stu.name, "test");
	stu.age = 20;
	unsigned int prio = atoi(argv[1]);
	mq_send(mqid, (const char*)&stu, sizeof(stu), prio);
	mq_close(mqid);
	
	return 0;
}


