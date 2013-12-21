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


int sem_create(key_t key)
{
	int semid;
	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}

int sem_open(key_t key)
{
	int semid;
	semid = semget(key, 0, 0);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}
/* 设置信号量的计数值 */
int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	int ret;
	ret = semctl(semid, 0, SETVAL, su);
	if (ret == -1)
		ERR_EXIT("sem_setval");

	return 0;
}


int sem_getval(int semid)
{
	int ret;
	ret = semctl(semid, 0, GETVAL, 0);
	if (ret == -1)
		ERR_EXIT("sem_getval");

	return ret;
}
/* 删除信号量集 */
int sem_d(int semid)
{
	int ret;
	ret = semctl(semid, 0, IPC_RMID, 0);
	if (ret == -1)
		ERR_EXIT("semctl");
	return 0;
}

int sem_p(int semid)
{
	struct sembuf sb = {0, -1, 0};
	int ret;
	ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_v(int semid)
{
	struct sembuf sb = {0, 1, 0};
	int ret;
	ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int semid;
/* pv操作之间的临界区，导致打印的字符一定是成对出现的 */
void print(char op_char)
{
	int pause_time;
	srand(getpid());
	int i;
	for (i = 0; i < 10; i++) {
		sem_p(semid);
		printf("%c", op_char);
		fflush(stdout);
		pause_time = rand() % 3;
		sleep(pause_time);
		printf("%c", op_char);
		fflush(stdout);
		sem_v(semid);
		pause_time = rand() % 2;
		sleep(pause_time);
	}

}

int main(void)
{

	semid = sem_create(IPC_PRIVATE);
	sem_setval(semid, 0);
	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fokr");

	if (pid > 0) {

		sem_setval(semid, 1);
		print('o');
		wait(NULL);
		sem_d(semid);

	}

	else {

		print('x');

	}


	return 0;
}


