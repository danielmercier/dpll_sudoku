.PHONY: all clean

all: a.out

a.out: main.o dpll.o
	g++ main.o dpll.o -o a.out

main.o: main.cpp
	g++ -c main.cpp

dpll.o: dpll.cpp
	g++ -c dpll.cpp

clean:
	rm -rf *.o a.out
