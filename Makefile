CC=c++11
DEPS=Options.h table.o

all: tool

tool:	$(DEPS) Tool.cpp
	g++ -std=$(CC) -g -o tool Tool.cpp table.o

table.o: Table.h Table.cpp
	g++ -std=$(CC) -g -c -o table.o Table.cpp

clean:
	rm *.o tool
