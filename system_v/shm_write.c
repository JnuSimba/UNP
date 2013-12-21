/*************************************************************************
	> File Name: basic.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Tue 12 Mar 2013 06:54:20 PM CST
 ************************************************************************/
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

typedef struct stu {
	char name[32];
	int age;
} STU;

int main(int argc, char* argv[])
{
	int shmid;
	shmid = shmget(1234, sizeof(STU), IPC_CREAT | 0666);
	if (shmid == -1)
		ERR_EXIT("shmget");

	STU* p;
	p = shmat(shmid, NULL, 0);
	if (p == (void*)-1)
		ERR_EXIT("shmat");

	strcpy(p->name, "lisi");
	p->age = 20;

	shmdt(p);

	return 0;
}


