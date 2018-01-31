all : proxy

proxy: proxy.o 
	g++ -g -o proxy proxy.o

proxy.o : proxy.cc
	g++ -g -c proxy.cc

clean :
	rm proxy
