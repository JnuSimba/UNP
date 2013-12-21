/*************************************************************************
	> File Name: basic.c
	> Author: Simba
	> Mail: dameng34@163.com 
	> Created Time: Tue 12 Mar 2013 06:54:20 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/mman.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main(void)
{
	shm_unlink("/xyz");
	return 0;
}


