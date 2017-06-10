lab2: main.o ThreadSafeKVStore.o HttpRequestParser.o ThreadPoolServer.o
	g++ -pthread -ggdb main.o ThreadSafeKVStore.o HttpRequestParser.o ThreadPoolServer.o -std=c++11 -o lab2

main.o: main.cpp ThreadSafeKVStore.hpp 
	g++ -pthread -ggdb -std=c++11 -c main.cpp

ThreadPoolServer.o: ThreadPoolServer.cpp ThreadSafeKVStore.hpp HttpRequestParser.hpp
	g++ -pthread -ggdb -std=c++11 -c ThreadPoolServer.cpp

HttpRequestParser.o: HttpRequestParser.cpp
	g++ -pthread -ggdb -std=c++11 -c HttpRequestParser.cpp

ThreadSafeKVStore.o: ThreadSafeKVStore.cpp
	g++ -pthread -ggdb -std=c++11 -c ThreadSafeKVStore.cpp

clean:
	rm *.o lab2