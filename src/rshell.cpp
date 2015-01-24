#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

static int run(char * cmd[], char *args[], int i);
void clean(char *args[]);
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
		char *cmd[1024];

		while(next != 0) {     // get arguments of each commands
			cmd[i] = next;
			char* begin = next;
			printf("%s\n", next);
	        
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
	cmd[i] = strtok(cmd[i], " \n");
    int a = 0;
	while (cmd[i] != NULL){
		args[a] = cmd[i];
		printf("%s\n", args[a]);
		++a;
		cmd[i] = strtok(NULL, " \n");}
		
		if (strcmp(args[0], "exit") == 0)  //inner exit command
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
				printf("Command not found");
            exit(1);											
		}
	    else {
	    	if( wait(&status) == -1)  //save the child process exit value
				perror("error");      // to statue and return it to parent 
	        return WEXITSTATUS(status);	//process    
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
    if(b!= NULL) {
	strncpy(e,d, b-a);  //get operator from input
	if (strstr(e, ";") !=NULL) return 0; 
	else if (strstr(e,"||")!= NULL) return 1;
	else if (strstr(e,"&&")!= NULL) return 2;
	else return 3;
	}
	return 0;
}


