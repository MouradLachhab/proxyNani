CXX = g++
CXXFLAGS = -g -std=c++11 -Wall


all : proxy

proxy: main.cc proxy.cc handler.cc proxy.h handler.h
	$(CXX) -o proxy main.cc proxy.cc handler.cc


clean :
	rm proxy