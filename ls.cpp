#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define KBLU  "\x1B[34;1m"
#define KGRN  "\x1B[32;1m"
#define BGRY  "\x1B[47;1m"
#define BG1   "\x1B[32;47;1m"
#define BG2   "\x1B[34;47;1m"
#define RESET "\033[0m"
using namespace std;

int flag_a = 0;
int flag_R = 0;
int flag_l = 0;
/*
 *  * This is a BARE BONES example of how to use opendir/readdir/closedir.  Notice
 *   * that there is no error checking on these functions.  You MUST add error 
 *    * checking yourself.
 *     */
void get_options(int argc, char* argv[]);
int list(char *path);
void print(struct stat mystat, char * name);
void print_l(struct stat mystat, char * name);
void print_group(gid_t gid);
void print_perm(struct stat st,int perm);
void print_user(uid_t uid);
void show(struct dirent * direntp, DIR * dirp);

int main(int argc,char *argv[] )
{
	get_options(argc, argv);
	int i = optind;
	if (argv[i] == NULL)
		list(argv[i]);
	else {
		while(argv[i] != NULL){
	     	list(argv[i]);
	    	printf("\n");
		    i++;
		}
	}	char time[64];

	return 0;
}

void get_options(int argc, char *argv[])
{
	int flag;
	opterr = 0;
	
	while((flag = getopt(argc, argv, "alR")) != -1)
	{
		switch(flag)
		{
			case 'a':
				flag_a = 1;
				break;
			case 'l':
				flag_l = 1;
				break;
			case 'R':
				flag_R = 1;
				break;
			case '?':
				cout<<"unknown flag"<< endl;
				break;
			default:
				break;
		}
	}
}

int list(char *path)
{
	char temp[100];
	char *dirName;
	struct stat mystat;
	DIR *dirp;
	struct dirent *direntp;	
	dirName = path;

	if (path == NULL){
		path = temp;
		getcwd(path, 100);
		dirName = path;
	}
	if (stat(path, &mystat)){
		perror("file not found");
	}
	if(S_ISREG(mystat.st_mode)){
		print(mystat,path);
		return 0;
	}
	else if(S_ISDIR(mystat.st_mode)){
		if((dirp = opendir(dirName)) == NULL){
			perror("open error");
			exit(1);
     	}
		show(direntp, dirp);			 	
	}
	else{
		printf("Cannot be listed.");
	}

}

void print(struct stat mystat, char*name){
	if((strcmp(name, ".") == 0)
	  ||(strcmp (name, "..") == 0)
	  || (name[0] == '.')) {
	  	if(S_ISDIR(mystat.st_mode))
	  		printf(BG2 "%s" RESET "  ", name);
	  	else if(S_ISREG(mystat.st_mode)){
	  		if(mystat.st_mode & S_IXOTH){
	  			printf(BG1 "%s" RESET "  ", name);
	  		}
	  	}
	  	else{
	  		printf(BGRY "%s" RESET "  ", name);
	  	}
	}
	else if(S_ISDIR(mystat.st_mode)){
		printf(KBLU "%s  " RESET, name);
	}
	else if(S_ISREG(mystat.st_mode)){
		if (mystat.st_mode & S_IXOTH){
			printf(KGRN "%s  " RESET, name);
		}
		else {
			printf("%s  " , name);
		}
	}
}

void print_perm(struct stat st,int perm)
{
		if (S_ISDIR(st.st_mode))  printf("d"); else printf("-");
		if ((perm & S_IRUSR) == S_IRUSR) printf("r"); else printf("-");
		if ((perm & S_IWUSR) == S_IWUSR) printf("w"); else printf("-");
		if ((perm & S_IXUSR) == S_IXUSR) printf("x"); else printf("-");
		if ((perm & S_IRGRP) == S_IRGRP) printf("r"); else printf("-");
		if ((perm & S_IWGRP) == S_IWGRP) printf("w"); else printf("-");
		if ((perm & S_IXGRP) == S_IXGRP) printf("x"); else printf("-");
		if ((perm & S_IROTH) == S_IROTH) printf("r"); else printf("-");
		if ((perm & S_IWOTH) == S_IWOTH) printf("w"); else printf("-");
		if ((perm & S_IXOTH) == S_IXOTH) printf("x"); else printf("-");
}

void print_user(uid_t uid)
{
		struct passwd *p;
		p = getpwuid(uid);
		printf("%-8s", p->pw_name);
}

void print_group(gid_t gid)
{
		struct group *g;
		g = getgrgid(gid);
		printf("%-8s", g->gr_name);
}

void print_l(struct stat mystat, char*name){
	if((strcmp(name, ".") == 0)
	  ||(strcmp (name, "..") == 0)
	  || (name[0] == '.')) {
	  	if(S_ISDIR(mystat.st_mode))
	  		printf(BG2 "%s" RESET "\n", name);
	  	else if(S_ISREG(mystat.st_mode)){
	  		if(mystat.st_mode & S_IXOTH){
	  			printf(BG1 "%s" RESET "\n", name);
	  		}
	  	}
	  	else{
	  		printf(BGRY "%s" RESET "\n", name);
	  	}
	}
	else if(S_ISDIR(mystat.st_mode)){
		printf(KBLU "%s\n" RESET, name);
	}
	else if(S_ISREG(mystat.st_mode)){
		if (mystat.st_mode & S_IXOTH){
			printf(KGRN "%s\n" RESET, name);
		}
		else{ 
			printf("%s\n" , name);
		}
	}
}

void show(struct dirent* direntp, DIR * dirp)
{
	struct stat st;
	char time[64];
	do{
		errno= 0;
		if((direntp = readdir(dirp)) != NULL){
			if((strcmp(direntp->d_name, ".") == 0)
			  || (strcmp(direntp->d_name, "..") == 0)
			  || (direntp->d_name[0] == '.')){
				if(flag_a == 0)
					continue;
			}
			if((stat(direntp->d_name, &st)) == -1)
				perror("stat error");
			if(flag_l == 0)
				print(st ,direntp->d_name);
			if(flag_l == 1){
				print_perm(st,st.st_mode);
				printf(" %d ",(int)st.st_nlink);
				print_user(st.st_uid);
				printf(" ");
				  print_group(st.st_gid);
				printf("%5li  ", (long) st.st_size);
				strftime(time, sizeof(time), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));
				printf("%s ", time);
				print_l(st, direntp->d_name);
			}
		}
	}while(direntp != NULL); 
		
	if (errno != 0){
		perror("read error");
		exit(1);
	}
	if ((closedir(dirp)) == -1)
		perror("close error");

}
