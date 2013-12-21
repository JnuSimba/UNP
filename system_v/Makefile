.PHONY: clean all
CC=gcc
CFLAGS=-Wall -g
BIN=msgget msgrmid msgstat msgset msgsend msgrecv echoser echocli mmap_write mmap_read \
	shm_write shm_read sem semtool print dinning
all:$(BIN) 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o *~ $(BIN)
