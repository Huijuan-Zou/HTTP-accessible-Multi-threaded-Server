lab2: main.o ThreadSafeKVStore.o ThreadSafeQueue.o HttpRequestParser.o ThreadPoolServer.o
	g++ -pthread -ggdb main.o ThreadSafeKVStore.o ThreadSafeQueue.o HttpRequestParser.o ThreadPoolServer.o -o lab2

main.o: main.cpp ThreadSafeKVStore.hpp 
	g++ -pthread -ggdb -c main.cpp

ThreadPoolServer.o: ThreadPoolServer.cpp ThreadSafeQueue.hpp ThreadSafeKVStore.hpp HttpRequestParser.hpp
	g++ -pthread -ggdb -std=c++11 -c ThreadPoolServer.cpp

HttpRequestParser.o: HttpRequestParser.cpp
	g++ -pthread -ggdb -std=c++11 -c HttpRequestParser.cpp

ThreadSafeQueue.o: ThreadSafeQueue.cpp
	g++ -pthread -ggdb -std=c++11 -c ThreadSafeQueue.cpp

ThreadSafeKVStore.o: ThreadSafeKVStore.cpp
	g++ -pthread -ggdb -std=c++11 -c ThreadSafeKVStore.cpp

clean:
	rm *.o lab2