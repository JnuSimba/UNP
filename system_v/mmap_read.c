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

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

typedef struct stu {
	char name[4];
	int age;
} STU;

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
		ERR_EXIT("open");

	
	STU* p;
	p = (STU*)mmap(NULL, sizeof(STU)*5, PROT_READ | PROT_WRITE, 
			MAP_SHARED, fd, 0);

	if (p == NULL)
		ERR_EXIT("mmap");

	int i;
	for (i = 0; i < 5; i++) {
		printf("name = %s age = %d\n", (p+i)->name, (p+i)->age);
	}
	munmap(p, sizeof(STU)*5);
	printf("exit...\n");
	return 0;
}


