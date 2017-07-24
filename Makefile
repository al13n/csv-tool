CC=c++11
COMMON=utility.h defs.h
DEPS=Options.h table.o

all: tool

tool:	$(COMMON) $(DEPS) Tool.cpp
	g++ -std=$(CC) -g -o tool Tool.cpp table.o

table.o: $(COMMON) Table.h Table.cpp
	g++ -std=$(CC) -g -c -o table.o Table.cpp

clean:
	rm *.o* tool
