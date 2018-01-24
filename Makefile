all : proxy

lab1-1 : proxy.c
	gcc -Wall -o proxy -lnsl -lsocket -lresol

clean :
	rm proxy
