#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

static int run(char * cmd[], char *args[], int i, int ret);
void clean(char *args[]);
int ope(char * cut, char * a, char * b, char* c);
using namespace std;

int main(int argc, char **argv, char **envp)
{
char line[1024];
char *args[1024];
char host[100];
char *user;
char *save;


while(1)
	{
		user = getlogin();
		gethostname(host, 100);
		printf("%s@%s>>", user, host);	        
        fflush(NULL);

        if(!fgets(line, 1024, stdin))
        	return 0;
		char * cut = line;
		int flag = 0;
		cut = strtok(line, "#");

	    char input[1024];
	    strncpy(input, cut, strlen(cut));
		char* next = strtok_r(cut, ";|&", &save);
		int ret = 0;
		
		int i = 0;
		char *cmd[1024];

		while(next != 0) {
			cmd[i] = next;
			char* begin = next;
			printf("%s\n", next);
	        
	        switch (flag)
		    {
				case(0): 
					ret = run(cmd, args, i, ret);
					break;
				case(1):
					if (ret == 1)  ret = run(cmd, args, i, ret);
					break;
				case(2):
					if (ret == 0)  ret = run(cmd, args,i, ret);
					break;
			}		
	
			clean(args);			
			i++;
			next = strtok_r(NULL, ";|&", &save);
			char *end = next;
			flag = ope(cut, begin, end, input);
		}

	}
	return 0;
}

static int run(char *cmd[], char *args[], int i, int ret)
{ 
	//int ret = 0;
	int status;
	cmd[i] = strtok(cmd[i], " \n");
    int a = 0;
	while (cmd[i] != NULL){
		args[a] = cmd[i];
		printf("%s\n", args[a]);
		++a;
		cmd[i] = strtok(NULL, " \n");}
		
		if (strcmp(args[0], "exit") == 0)		
			exit(0);
		else
		{
			int pid = fork();
			if(pid == -1){
			perror("sdf");
			exit(1);
		}			
		else if (pid == 0)
		{
			if (execvp(args[0], args) != 0)
				("Command not found");
            exit(1);											
		}
	    else {
	    	if( wait(&status) == -1)
				perror("error");
	        return WEXITSTATUS(status);	    
	    }	    
	}
}

void clean(char *args[]) 
{
	int i = 0;
	while (args[i] != NULL)
	{
		args[i] = NULL;
		i++;
	}
}

int ope(char *cut, char * a , char * b, char* c){
	char *d = c;
	char e[1024];
	d= d + (a - cut);
//	printf("%s\n", d);
    if(b!= NULL) {
	strncpy(e,d, b-a);
	if (strstr(e, ";") !=NULL) return 0;
	else if (strstr(e,"||")!= NULL) return 1;
	else if (strstr(e,"&&")!= NULL) return 2;
	}
	else return 0;
}


