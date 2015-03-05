all: rshell ls
		mkdir bin
		mv ./rshell ./bin/rshell
		mv ./ls ./bin/ls
rshell: ./src/rshell.cpp
		g++ -Wall -Werror -ansi -pedantic -o rshell ./src/rshell.cpp
ls: ./src/ls.cpp	
		g++ -Wall -Werror -ansi -pedantic -o ls ./src/ls.cpp
clean: ./bin/rshell ./bin/ls
		rm -rf ./bin
		
