#include "ipc.h"

int sem_create(key_t key)
{
	int semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}

int sem_open(key_t key)
{
	int semid = semget(key, 0, 0);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}

int sem_p(int semid)
{
	struct sembuf sb = {0, -1, 0};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_v(int semid)
{
	struct sembuf sb = {0, 1, 0};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_d(int semid)
{
	int ret = semctl(semid, 0, IPC_RMID, 0);
	/*
	if (ret == -1)
		ERR_EXIT("semctl");
	*/
	return ret;	
}

int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	int ret = semctl(semid, 0, SETVAL, su);
	if (ret == -1)
		ERR_EXIT("semctl");

	//printf("value updated...\n");
	return ret;
}

int sem_getval(int semid)
{
	int ret = semctl(semid, 0, GETVAL, 0);
	if (ret == -1)
		ERR_EXIT("semctl");

	//printf("current val is %d\n", ret);
	return ret;
}

int sem_getmode(int semid)
{
        union semun su;
        struct semid_ds sem;
        su.buf = &sem;
        int ret = semctl(semid, 0, IPC_STAT, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("current permissions is %o\n",su.buf->sem_perm.mode);
        return ret;
}

int sem_setmode(int semid,char* mode)
{
        union semun su;
        struct semid_ds sem;
        su.buf = &sem;

        int ret = semctl(semid, 0, IPC_STAT, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("current permissions is %o\n",su.buf->sem_perm.mode);
        sscanf(mode, "%o", (unsigned int*)&su.buf->sem_perm.mode);
        ret = semctl(semid, 0, IPC_SET, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("permissions updated...\n");

        return ret;
}
