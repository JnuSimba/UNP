pingy: ping.o pingy.o
	cc -o pingy pingy.o ping.o

pingy.o: PingPro.c ping.c ping.h
	cc -c PingPro.c -o pingy.o

ping.o: ping.c ping.h
	cc -c ping.c

clean:
	rm -rf pingy *.o