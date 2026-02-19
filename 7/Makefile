build:
	g++ -Wall -pedantic -fsanitize=address -Wno-long-long -std=c++20 -g main.cpp -o ./bin/run

run: build
	exec ./bin/run

debug: build 
	gdb ./bin/run 
	
