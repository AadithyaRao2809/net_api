dev: 
	g++ -std=c++20 -g -DDEBUG -o bin/test src/main.cpp 
prod:
	g++ -std=c++20 -O3 -o bin/net_api src/main.cpp
test : 
	./bin/test

