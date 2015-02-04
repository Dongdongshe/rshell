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
void show( DIR * dirp);
void enq(char *);
void deq();
char* retDir();
struct node {
	char path[512];
	struct node *next;
} *front, *rear, *temp, *front1;


int main(int argc,char *argv[] )
{
		char buf[512];
			size_t t= 512;
			if((getcwd(buf,t))==NULL)
				perror("getcwd error");

	get_options(argc, argv);
	int i = optind;
	if (argv[i] == NULL)
		list(argv[i]);
	else {
		while(argv[i] != NULL){
		     	list(argv[i]);
			if((chdir(buf)) == -1)
				perror("chdir error");
		    i++;
		}
	}

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
	char *dirName = path;
	struct stat mystat;
	DIR *dirp;

	if (path == NULL){
		path = temp;
		if((getcwd(path, 100)) == NULL)
			perror("getcwd error");
		dirName = path;
	}

	if (stat(path, &mystat)){
		perror("file not found");
	}
	if(S_ISREG(mystat.st_mode)){
		printf("%s\n", path);
		print(mystat,path);
		return 0;
	}
	else if(S_ISDIR(mystat.st_mode)){
		front = (struct node *)malloc(sizeof(struct node));
		strncpy(front->path,dirName,512);
		rear = front;
		while(front != NULL){

			if((chdir(front->path)) != 0)
					perror("chdir error");

			if((dirp = opendir(front->path)) == NULL){
				perror("open error");
				exit(1);
		 	}
		 	if(flag_R == 1)
				printf("%s\n",front->path);
			show(dirp);
			deq();
		}
	}
	return 0;
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
		printf("%-6s", p->pw_name);
}

void print_group(gid_t gid)
{
		struct group *g;
		g = getgrgid(gid);
		printf("%-6s", g->gr_name);
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

void show( DIR * dirp)
{

	struct dirent *direntp;	
	struct stat st;
	char time[64];

	while((direntp = readdir(dirp)) != NULL){
		if((stat(direntp->d_name, &st)) == -1)
			perror("stat error");

		if((strcmp(direntp->d_name, ".") == 0)
		  || (strcmp(direntp->d_name, "..") == 0)
		  || (direntp->d_name[0] == '.')){
			if(flag_a == 0)
				continue;
		}
		if(flag_R == 1){
			if((strcmp(direntp->d_name, ".") != 0)
		      && (strcmp(direntp->d_name, "..") != 0)){
		      	if(S_ISDIR(st.st_mode)){
		      		char buf[512];
		      		size_t size=512;
		      		if((getcwd(buf, size)) == NULL)
		      			perror("getcwd error");
					strcat(buf,"/");
		      		strcat(buf,direntp->d_name);
		      		enq(buf);
		      	}
		    }
		}
		if(flag_l == 0)
			print(st ,direntp->d_name);
		if(flag_l == 1){
			print_perm(st,st.st_mode);
			printf(" %4d ",(int)st.st_nlink);
			print_user(st.st_uid);
			printf(" ");
			print_group(st.st_gid);
			printf("%8li  ", (long) st.st_size);
			strftime(time, sizeof(time), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));
			printf("%s ", time);
			print_l(st, direntp->d_name);
		}
	}
	
	if ((closedir(dirp)) == -1)
		perror("close error");
	printf("\n");
	printf("\n");


}

void enq(char *path)
{

	    temp=(struct node *)malloc(1*sizeof(struct node));
	    rear->next = temp;
	    strncpy(temp->path,path,512);
	    temp->next = NULL;
	    rear = temp;
	    
}

void deq()
{
	front1 = front;
	if (front1 == NULL){
	   // printf("\n Error: Trying to display elements from empty queue");
	    return;
	}
	else
	    if (front1->next != NULL){
	    	front1 = front1->next;
	    //	printf("\n Dequed value : %d", front->info);
	        free(front);
	        front = front1;
	     }                              
         else{
	         //printf("\n Dequed value : %d", front->info);	         
	         free(front);
	         front = NULL;	         	     	 
	     	 rear = NULL;	         	     	 
	     }    	     	 
}

char * frontelement()
{
	if ((front != NULL) && (rear != NULL))
	    return(front->path);
	else
	    return NULL;
}


