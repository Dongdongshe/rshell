#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include "Timer.h"
#define COPYMORE 0644
void streamCopy(char *s, char *d)
{
	std::ifstream is(s);
   	char ch;
	std::ofstream output;
 	output.open(d);
	while(is.get(ch)){
		output.put(ch);
}
	 is.close();
	output.close();
}
 
int charCopy(char *s, char *d)
{
	int inp, outp, charsz;
	char buf[1];
	if((inp = open(s,O_RDONLY)) == -1)
		perror("Cannot open");
	if((outp = creat(d, COPYMORE)) == -1)
		perror("Cannot create");
	while((charsz = read(inp, buf, 1)) > 0)
	{
		if(write(outp, buf, charsz) != charsz)
			perror("Write error");
		if(charsz == -1)
			perror("Read error");
	}
	if(close(inp == -1 || close(outp) == -1))
		perror("Error closing files");
	return 1;
}
int bufCopy(char *s, char *d)
{
	int inp, outp, charsz;
	char buf[BUFSIZ];
	if((inp = open(s,O_RDONLY)) == -1)
		perror("Cannot open");
	if((outp = creat(d, COPYMORE)) == -1)
		perror("Cannot create");
	while((charsz = read(inp, buf, BUFSIZ)) > 0)
	{
		if(write(outp, buf, charsz) != charsz)
			perror("Write error");
		if(charsz == -1)
			perror("Read error");
	}
	if(close(inp == -1 || close(outp) == -1))
		perror("Error closing files");
	return 1;
}

int main(int argc, char* argv[])
{
	int flag = getopt(argc, argv, "a");
	int i = optind;
	char *src = argv[i];

	using namespace std;

	char *dst = argv[i+1];
	struct stat st;
	lstat(src, &st);
	if(S_ISDIR(st.st_mode)){
		printf("Source is a directory");
		return 1;
	}

	if( access( dst, F_OK ) == -1 ) {

		if (flag == 'a'){
			Timer t1;
			double eTime1;
			t1.start();
			streamCopy(src, dst);
			t1.elapsedUserTime(eTime1);
			cout << "streamCopy"<< endl <<eTime1 << endl;

			Timer t2;
			double eTime2;
			t2.start();
			charCopy(src, dst);
			t2.elapsedUserTime(eTime2);
			cout << "charCopy"<< endl <<eTime2 << endl;

			Timer t3;
			double eTime3;
			t3.start();
			bufCopy(src, dst);
			t3.elapsedUserTime(eTime3);
			cout << "bufCopy"<< endl <<eTime3 << endl;
		}
		else {
			bufCopy(src, dst);
		}
	 }
	 
	 else {
	 	 printf("file exists");
	 	 return 1;
	 }


	/*if(dst)
		perror("File already exists");
	*/


	return 0;
}
