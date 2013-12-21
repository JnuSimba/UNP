.PHONY: clean all
CC=gcc
CFLAGS=-Wall -g
BIN=pthread_create echoser echocli pthread_attr pthread_tsd pthread_sem_mutex pthread_mutex_cond
all:$(BIN) 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
pthread_create:pthread_create.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
echoser:echoser.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
echocli:echocli.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
pthread_attr:pthread_attr.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
pthread_tsd:pthread_tsd.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
pthread_sem_mutex:pthread_sem_mutex.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
pthread_mutex_cond:pthread_mutex_cond.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
clean:
	rm -f *.o *~ $(BIN)
