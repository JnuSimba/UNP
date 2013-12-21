#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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
	struct sembuf sb = {0, -1, /*0IPC_NOWAIT*/SEM_UNDO};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_v(int semid)
{
	struct sembuf sb = {0, 1, /*0*/SEM_UNDO};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_d(int semid)
{
	int ret = semctl(semid, 0, IPC_RMID, 0);
	if (ret == -1)
		ERR_EXIT("semctl");
	return ret;	
}

int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	int ret = semctl(semid, 0, SETVAL, su);
	if (ret == -1)
		ERR_EXIT("semctl");

	printf("value updated...\n");
	return ret;
}

int sem_getval(int semid)
{
	int ret = semctl(semid, 0, GETVAL, 0);
	if (ret == -1)
		ERR_EXIT("semctl");

	printf("current val is %d\n", ret);
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

void usage(void)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "semtool -c\n");
	fprintf(stderr, "semtool -d\n");
	fprintf(stderr, "semtool -p\n");
	fprintf(stderr, "semtool -v\n");
	fprintf(stderr, "semtool -s <val>\n");
	fprintf(stderr, "semtool -g\n");
	fprintf(stderr, "semtool -f\n");
	fprintf(stderr, "semtool -m <mode>\n");
}

int main(int argc, char *argv[])
{
	int opt;

	opt = getopt(argc, argv, "cdpvs:gfm:");
	if (opt == '?')
		exit(EXIT_FAILURE);
	if (opt == -1)
	{
		usage();
		exit(EXIT_FAILURE);
	}

	key_t key = ftok(".", 's');
	int semid;
	switch (opt)
	{
	case 'c':
                sem_create(key);
                break;
        case 'p':
                semid = sem_open(key);
                sem_p(semid);
                sem_getval(semid);
                break;
        case 'v':
                semid = sem_open(key);
                sem_v(semid);
		sem_getval(semid);
                break;
        case 'd':
                semid = sem_open(key);
                sem_d(semid);
                break;
        case 's':
                semid = sem_open(key);
                sem_setval(semid, atoi(optarg));
                break;
        case 'g':
                semid = sem_open(key);
                sem_getval(semid);
                break;
        case 'f':
                semid = sem_open(key);
                sem_getmode(semid);
                break;
        case 'm':
		semid = sem_open(key);
		sem_setmode(semid, argv[2]);
		break;
	}

	return 0;
}

