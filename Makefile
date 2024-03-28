all: compile run
compile: 
	g++ -o bin/test src/main.cpp 
run : 
	./bin/test
