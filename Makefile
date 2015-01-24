all: rshell

rshell: ./src/rshell.cpp
		g++ -Wall -Werror -ansi -pedantic -o rshell ./src/rshell.cpp
	
