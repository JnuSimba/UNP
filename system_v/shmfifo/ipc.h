#ifndef _IPC_H_
#define _IPC_H_

#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

union semun {
	int val;                  /* value for SETVAL */
	struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;    /* array for GETALL, SETALL */
							  /* Linux specific part: */
	struct seminfo *__buf;    /* buffer for IPC_INFO */
};

int sem_create(key_t key);
int sem_open(key_t key);

int sem_p(int semid);
int sem_v(int semid);
int sem_d(int semid);
int sem_setval(int semid, int val);
int sem_getval(int semid);
int sem_getmode(int semid);
int sem_setmode(int semid,char* mode);


#endif /* _IPC_H_ */
