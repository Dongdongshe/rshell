all: rshell ls
		mkdir bin
		cp ./rshell ./bin/rshell
		cp ./ls ./bin/ls
rshell: ./src/rshell.cpp
		g++ -Wall -Werror -ansi -pedantic -o rshell ./src/rshell.cpp
ls: ./src/ls.cpp	
		g++ -Wall -Werror -ansi -pedantic -o ls ./src/ls.cpp
