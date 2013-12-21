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
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/wait.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)
            
union semun {
	int val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */ 
};

int semid;

#define DELAY (rand() % 5 + 1)

void wait_for_2fork(int no)
{
	int left = no;
	int right = (no + 1) % 5;

	struct sembuf buf[2] = {
		{left, -1, 0},
		{right, -1, 0}
	};

	semop(semid, buf, 2);
}

void free_2fork(int no)
{
	int left = no;
	int right = (no + 1) % 5;

	struct sembuf buf[2] = {
		{left, 1, 0},
		{right, 1, 0}
	};

	semop(semid, buf, 2);
}

void philosopere(int no)
{
	srand(getpid());
	for (; ;) {
		
		printf("%d is thinking\n", no);
		sleep(DELAY);
		printf("%d is hungry\n", no);
		wait_for_2fork(no);
		printf("%d is eating\n", no);
		sleep(DELAY);
		free_2fork(no);
	}
}


int main(void)
{

	semid = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
	if (semid == -1)
		ERR_EXIT("semget");
	union semun su;
	su.val = 1;
	int i;
	for (i = 0; i < 5; i++) {
		semctl(semid, i, SETVAL, su);
	}

	int no = 0;
	pid_t pid;
	for (i = 1; i < 5; i++) {
		pid = fork();
		if (pid == -1)
			ERR_EXIT("fork");

		if (pid == 0) {
			no = i;
			break;
		}
	}

	philosopere(no);

	return 0;
}


