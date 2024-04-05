dev: 
	g++ -std=c++20 -g -DDEBUG -o bin/test src/main.cpp 
release:
	g++ -std=c++20 -O3 -o bin/net_api src/main.cpp
test : 
	./bin/test
run:
	./bin/net_api
clean:
	rm -f bin/*

