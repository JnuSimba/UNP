tracerty: main.o tracert.o
	cc -o tracerty main.o tracert.o

main.o: main.c tracert.o
	cc -c main.c -o main.o

tracert.o: tracert.c tracert.h
	cc -c tracert.c tracert.h

clean:
	rm -rf tracerty *.o