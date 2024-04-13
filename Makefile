.PHONY: all test run clean
all: dev test
dev: 
	g++ -std=c++20 -g -DDEBUG -o bin/test src/main.cpp 
client:
	g++ -std=c++20 -g -o bin/client src/main.cpp
server:
	g++ -std=c++20 -g -DSERVER -o bin/server src/main.cpp
dclient:
	g++ -std=c++20 -g -DDEBUG -o bin/client src/main.cpp
dserver:
	g++ -std=c++20 -g -DSERVER -DDEBUG -o bin/server src/main.cpp
release:
	g++ -std=c++20 -O3 -o bin/net_api src/main.cpp
test : 
	./bin/test
run:
	./bin/net_api
clean:
	rm -f bin/*

