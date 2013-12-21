.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=shmfifo_send shmfifo_recv shmfifo_free
OBJS1=shmfifo_send.o shmfifo.o ipc.o
OBJS2=shmfifo_recv.o shmfifo.o ipc.o
OBJS3=shmfifo_free.o shmfifo.o ipc.o
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
shmfifo_send:$(OBJS1)
	$(CC) $(CFLAGS) $^ -o $@
shmfifo_recv:$(OBJS2)
	$(CC) $(CFLAGS) $^ -o $@
shmfifo_free:$(OBJS3)
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f *.o $(BIN)

