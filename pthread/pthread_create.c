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
#include<pthread.h>
#include<string.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

void* routine(void *arg)
{
	int i;
	for (i = 0; i < 20; i++) {
		printf("B");
		fflush(stdout);
		usleep(20);
	/*
		if (i == 3)
			pthread_exit("ABC");
		*/
	}
	return "DEF";
}

int main(void)
{
	pthread_t tid;
	int ret;
	if ((ret = pthread_create(&tid, NULL, routine, NULL)) != 0) {
		fprintf(stderr, "pthread create: %s\n", strerror(ret));
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < 20; i++) {
		printf("A");
		fflush(stdout);
		usleep(20);
	}
	
	void* value;
	if ((ret = pthread_join(tid, &value)) != 0) {
		fprintf(stderr, "pthread create: %s\n", strerror(ret));
		exit(EXIT_FAILURE);
	}

	printf("\n");

	printf("return msg=%s\n", (char*)value);
	return 0;
}


