#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
static int run(char *);
void clean(char *args[]);

void execute(char ** args, int flag, char * dup);
int ope(char * cut, char * a, char * b, char* c);
int main(int argc, char **argv, char **envp)
{
char line[1024]; 
char host[100];
char *user;
char *save;


while(1)
	{
		user = getlogin();    //get user name
		gethostname(host, 100);    //get os name
		printf("%s@%s>>", user, host);	        //print machine info
        fflush(NULL);

        if(!fgets(line, 1024, stdin))       
        	return 0;
		char * cut = line;            
		int flag[64];
		char input[1024];

		cut = strtok(line, "#");   //cut off characters following #
	    strncpy(input, cut, strlen(cut));   //save the input to a array		
		char* next = strtok_r(cut, ";|&", &save); //devide input into commands
		int ret = 0;		
		int i = 0;
		char cmd[64][1024];
	    while(next != 0) {
	    	strncpy(cmd[i],next,1024);
	    	char *begin = next;
			next = strtok_r(NULL, ";|&", &save);
			char *end = next;
			flag[i] = ope(cut, begin, end, input); //detect operator
			i++;
		}
		int j;
		int n = i;
		i = 0;
		for(j=0;j < n;j++) {    
			char * execu= cmd[i];
	        switch (flag[i]) //run next command based on operator and return
		    {             //of former command
				case(0):  // case of ;
					ret = run(execu);
					break;
				case(1): // case of ||
					if (ret == 1)  ret = run(execu);
					break;
				case(2):// case of &&
					if (ret == 0)  ret = run(execu);
				case(3): // case of the others
					printf("wrong operator");
					break;
			}		
	
			i++;
		}

	}
	return 0;
}

static int run(char *execu) // execute command
{ 
	int status;
	char * dup;
	char *save;
	const char *e;
	char buf[1024];
	int flag;
	char *args[1024];

	if (strstr(execu, "<<<")!= NULL)
		flag = 4;
	else if (strstr(execu, ">>")!= NULL)
		flag = 3;	
	else if (strstr(execu, "1>")!= NULL)
		flag = 5;
	else if (strstr(execu, "<")!= NULL)
		flag = 1;
	else if (strstr(execu, "2>")!= NULL)
		flag = 6;
	else if (strstr(execu, ">")!= NULL)
		flag = 2;
	else 
		flag = 0;
	execu = strtok(execu, "<12>");
	dup = strtok(NULL, "<12>");
	dup = strtok(dup, " \n");
	execu = strtok_r(execu, " \n", &save);
    int a = 0;
	while (execu != NULL){
		args[a] = execu;
		++a;
		execu = strtok_r(NULL, " \n", &save);
	}
		
	if (strcmp(args[0], "exit") == 0)  //inner exit command
		exit(0);
	else{
		int pid = fork();
		if(pid == -1){
			perror("fork error");
			exit(1);
		}			
		else if (pid == 0)
		{
			execute(args, flag, dup);	
			clean(args);  //clean arguments 
			exit(1);											
		}
		else {
		 	if( wait(&status) == -1)  //save the child process exit value
				perror("error");      // to statue and return it to parent 
			return WEXITSTATUS(status);	//process    
		}	    
	}
}

void execute(char ** args, int flag, char * dup)
{
	int in,out;
	char buf[1024];
	switch (flag)
	{
		case 0:
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			break;
		case 1:
			in = open(dup, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			dup2(in, 0);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(in);
			break;
		case 2:
			out = open(dup, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			dup2(out, 1);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(out);
			break;
		case 3:	
			out = open(dup, O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
			dup2(out, 1);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(out);
			break;
		case 4:
			int t;
			t = strlen(dup);
			strncpy(buf, dup,t-1);
			in  = creat("temp.txt", S_IRUSR|S_IWUSR);
			write(in, buf, t-3);
			close(in);
			in = open("temp.txt", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			dup2(in, 0);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(in);
			break;
		case 5:
			out = open(dup, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			dup2(out, 1);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(out);
			break;
		case 6:
			out = open(dup, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			dup2(out, 2);
			if (execvp(args[0], args) != 0)
				printf("Command not found");
			close(out);
			break;
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
    if(b!= NULL) {
	strncpy(e,d, b-a);  //get operator from input
	if (strstr(e, ";") !=NULL) return 0; 
	else if (strstr(e,"|")!= NULL) return 4;
	else if (strstr(e,"||")!= NULL) return 1;
	else if (strstr(e,"&&")!= NULL) return 2;
	else return 3;
	}
	return 0;
}




