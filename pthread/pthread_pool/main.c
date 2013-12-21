#include "threadpool.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void* mytask(void *arg)
{
	printf("thread 0x%x is working on task %d\n", (int)pthread_self(), *(int*)arg);
	sleep(1);
	free(arg);
	return NULL;
}

int main(void)
{
	threadpool_t pool;
	threadpool_init(&pool, 3);

	int i;
	for (i=0; i<10; i++)
	{
		int *arg = (int *)malloc(sizeof(int));
		*arg = i;
		threadpool_add_task(&pool, mytask, arg);
	}

	//sleep(15);
	threadpool_destroy(&pool);
	return 0;
}
