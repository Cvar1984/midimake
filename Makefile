CXX ?=g++
CXXFLAGS += -Wall -g -std=c++0x
PREFIX ?=/usr

main: main.cpp
clean:
	rm -f main *mid
install: main
	install main ${PREFIX}/bin
uninstall:
	rm -f ${PREFIX}/bin/main
