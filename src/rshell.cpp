#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
static int run(char * cmd[], char *args[], int i);
void clean(char *args[]);

void execute(char ** args, int flag, char * dup);
int ope(char * cut, char * a, char * b, char* c);
int main(int argc, char **argv, char **envp)
{
char line[1024]; 
char *args[1024];
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
		int flag = 0;
		char input[1024];

		cut = strtok(line, "#");   //cut off characters following #
	    strncpy(input, cut, strlen(cut));   //save the input to a array		
		char* next = strtok_r(cut, ";|&", &save); //devide input into commands
		int ret = 0;		
		int i = 0;
		char* cmd[1024];
		while(next != 0) {     // get arguments of each commands
			cmd[i] = next;
			char* begin = next;
	        
	        switch (flag) //run next command based on operator and return 
		    {             //of former command
				case(0):  // case of ;
					ret = run(cmd, args, i);
					break;
				case(1): // case of ||
					if (ret == 1)  ret = run(cmd, args, i);
					break;
				case(2):// case of &&
					if (ret == 0)  ret = run(cmd, args,i);
				case(3): // case of the others
					printf("wrong operator");
					break;
			}		
	
			clean(args);  //clean arguments 
			i++;
			next = strtok_r(NULL, ";|&", &save);
			char *end = next;
			flag = ope(cut, begin, end, input); //detect operator
		}

	}
	return 0;
}

static int run(char *cmd[], char *args[], int i) // execute command
{ 
	int status;
	char * dup;
	char *save;
	const char *e;
	char buf[1024];
	int flag;

	if (strstr(cmd[i], "<<<")!= NULL)
		flag = 4;
	else if (strstr(cmd[i], ">>")!= NULL)
		flag = 3;	
	else if (strstr(cmd[i], "1>")!= NULL)
		flag = 5;
	else if (strstr(cmd[i], "<")!= NULL)
		flag = 1;
	else if (strstr(cmd[i], "2>")!= NULL)
		flag = 6;
	else if (strstr(cmd[i], ">")!= NULL)
		flag = 2;
	else 
		flag = 0;
	cmd[i] = strtok(cmd[i], "<12>");
	dup = strtok(NULL, "<12>");
	dup = strtok(dup, " \n");
	cmd[i] = strtok_r(cmd[i], " \n", &save);
    int a = 0;
	while (cmd[i] != NULL){
		args[a] = cmd[i];
		++a;
		cmd[i] = strtok_r(NULL, " \n", &save);
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
	//		if (execvp(args[0], args) != 0)
	//			printf("Command not found");
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
			strncpy(buf, dup+1,t-3);
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
//	d= d + (a - cut);   
    if(b!= NULL) {
	strncpy(e,d, b-a);  //get operator from input
	if (strstr(e, ";") !=NULL) return 0; 
	else if (strstr(e,"||")!= NULL) return 1;
	else if (strstr(e,"|")!= NULL) return 1;
	else if (strstr(e,"&&")!= NULL) return 2;
	else return 3;
	}
	return 0;
}



