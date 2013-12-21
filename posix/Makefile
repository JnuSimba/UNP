.PHONY: clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mq_open mq_unlink mq_getattr mq_send mq_recv mq_notify shm_open shm_unlink shm_write shm_read
all:$(BIN) 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
mq_open: mq_open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_unlink: mq_unlink.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_getattr: mq_getattr.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_send: mq_send.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_recv: mq_recv.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
mq_notify: mq_notify.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_open: shm_open.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_unlink: shm_unlink.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_write: shm_write.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
shm_read: shm_read.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
clean:
	rm -f *.o *~ $(BIN)
