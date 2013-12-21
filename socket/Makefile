.PHONY: clean all
CC=gcc
CFLAGS=-Wall -g
BIN=byteorder addr_in echoser echocli echoser_fork p2pser p2pcli echoser_stick echocli_stick \
    getiplist echoser_recv_peek echocli_recv_peek echocli_5sock echocli_select echoser_select \
	echocli_select_shutdown echoser_timeout echocli_timeout nofilelimit echoser_poll echoser_epoll \
    conntest echoser_udp  echocli_udp trunc charser charcli \
	echoser_unixdomain echocli_unixdomain socketpair uxdomsock_sendfd
all:$(BIN) 
echoser_recv_peek: echoser_recv_peek.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_recv_peek: echocli_recv_peek.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echoser_stick: echoser_stick.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_stick: echocli_stick.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_5sock: echocli_5sock.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_select: echocli_select.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echoser_select: echoser_select.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_select_shutdown: echocli_select_shutdown.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
echocli_timeout: echocli_timeout.o sysutil.o
	$(CC) $(CFLAGS) $^ -o $@
echoser_poll: echoser_poll.o read_write.o
	$(CC) $(CFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:%.cpp
	g++ $(CFLAGS) -c $< -o $@
echoser_epoll: echoser_epoll.o read_write.o sysutil.o
	g++ $(CFLAGS) $^ -o $@
charser: charser.o 
	g++ $(CFLAGS) $^ -o $@
charcli: charcli.o 
	g++ $(CFLAGS) $^ -o $@

clean:
	rm -f *.o *~ $(BIN)
