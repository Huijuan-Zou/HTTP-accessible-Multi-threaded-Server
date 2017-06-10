lab1: main.o ThreadSafeKVStore.o
	g++ -pthread -ggdb main.o ThreadSafeKVStore.o -o lab1

main.o: main.cpp ThreadSafeKVStore.hpp
	g++ -pthread -ggdb -std=c++11 -c main.cpp

ThreadSafeKVStore.o: ThreadSafeKVStore.cpp
	g++ -pthread -ggdb -std=c++11 -c ThreadSafeKVStore.cpp

clean:
	rm *.o lab1
