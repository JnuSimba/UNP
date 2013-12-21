#include "shmfifo.h"

typedef struct stu
{
	char name[32];
	int age;
} STU;

int main(void)
{
	shmfifo_t *fifo = shmfifo_init(1234, sizeof(STU), 3);

	STU s;
	memset(&s, 0, sizeof(STU));
	int i;
	
	for (i=0; i<5; i++)
	{
		shmfifo_get(fifo, &s);
		printf("name = %s age = %d\n", s.name, s.age);
	}
	
	return 0;
}
