#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
static int run(char *);
int run_pipe(char cmd[64][1024],int i, int pp[2]);
void clean(char *args[]);
int ret = 0;
int num  = 0;
int flag[64];
int savestdin;
int in,out;
int execute(char ** args, int flag, char * dupchar);
int ope(char * cut, char * a, char * b, char* c);
int main(int argc, char **argv, char **envp)
{

	char line[1024]; 
	char host[100];
//	char *user;
	char *save;
	
	while(1){
		//if(NULL == (user = getlogin()))
	//		perror("getlogin");    //get user name
		if (-1 == (gethostname(host, 100)))
			perror("gethostname");    //get os name
		printf("%s@%s>>", "sherman", host);	        //print machine info
        fflush(NULL);

        if(!fgets(line, 1024, stdin))       
        	return 0;
		char * cut = line;            
		char input[1024];

		cut = strtok(line, "#");   //cut off characters following #
	    strncpy(input, cut, strlen(cut));   //save the input to a array		
		char* next = strtok_r(cut, ";|&", &save); //devide input into commands
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
		num  = n;
		i = 0;
		int pp[2];
		if (flag[0] == 4){
			run_pipe(cmd,i,pp);
			if (-1 == dup2(savestdin,0))
				perror("dup2 error");
		}
		else {
		for(j=0;j < n;j++) {    
			char * execu= cmd[j];
			if (j==0)
				ret = run(execu);
			else{				
				switch (flag[j-1]) //run next command based on ope
			    {             //of former command
					case(0):  // case of ;
						ret = run(execu);
						break;
					case(1): // case of ||
						if (ret == 1){
							ret = run(execu);
						}
						break;
					case(2):// case of &&
						if (ret == 0){
							ret = run(execu);
						}
					case(3): // case of the others
						printf("wrong operator");
						break;
				}		
			}
		}
		}
	}
	return 0;
}

static int run(char *execu){ 
	int status;
	char * dupchar;
//	char * dupchar1;
	char *save;
	int flag;
	char *args[1024];
	char * pch;

	if((pch = strpbrk(execu,"<12>"))!=NULL){
		if (strstr(pch, "<<<")!= NULL){
			char str1[4];
			strncpy(str1,pch,3);
			str1[3] = '\0';
			if ((strcmp(str1,"<<<") == 0) &&(strpbrk(pch+3,"<>") == NULL))
				flag = 4;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, "1>>")!= NULL){
			char str8[4];
			strncpy(str8,pch,3);
			str8[3] = '\0';
			if ((strcmp(str8,"1>>") == 0) &&(strpbrk(pch+3,"<>") == NULL))
				flag = 8;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, "2>>")!= NULL){
			char str7[4];
			strncpy(str7,pch,3);
			str7[3] = '\0';
			if ((strcmp(str7,"2>>") == 0) &&(strpbrk(pch+3,"<>") == NULL))
				flag = 7;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, ">>")!= NULL){
			char str2[3];
			strncpy(str2,pch,2);
			str2[2] = '\0';
			if ((strcmp(str2,">>") == 0) &&(strpbrk(pch+2,"<>") == NULL))
				flag = 3;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, "1>")!= NULL){
			char str3[3];
			strncpy(str3,pch,2);
			str3[2] = '\0';
			if ((strcmp(str3,"1>") == 0) &&(strpbrk(pch+2,"<>") == NULL))
				flag = 5;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, "<")!= NULL){
			char str4[2];
			strncpy(str4,pch,1);
			str4[1] = '\0';
			if ((strcmp(str4,"<") == 0) &&(strpbrk(pch+1,"<>") == NULL))
				flag = 1;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, "2>")!= NULL){
			char str5[3];
			strncpy(str5,pch,2);
			str5[2] = '\0';
			if ((strcmp(str5,"2>") == 0) &&(strpbrk(pch+2,"<>") == NULL))
				flag = 6;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
		else if (strstr(pch, ">")!= NULL){
			char str6[2];
			strncpy(str6,pch,1);
			str6[1] = '\0';
			if ((strcmp(str6,">") == 0) &&(strpbrk(pch+1,"<>") == NULL))
				flag = 2;		
			else {
				printf("invalid IO redirection\n");
				return 1;}
		}
	}	
	else flag = 0;
	execu = strtok(execu, "<12>");
	dupchar = strtok(NULL, "<12>");
//	dupchar = strtok(dupchar, " \n");
	execu = strtok_r(execu, " \n", &save);
    int a = 0;
	clean(args);  //clean arguments 
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
			execute(args, flag, dupchar);	
			exit(1);											
		}
		else {
		 	if( wait(&status) == -1)  //save the child process exit value
				perror("error");      // to statue and return it to parent 
			return WEXITSTATUS(status);	//process    
		}	    
	}
}

int execute(char ** args, int flag, char * dupchar) //execute different dup 
{
	char buf[1024];
	switch (flag)
	{
		case 0:
			if (execvp(args[0], args) != 0)
				perror("Command not found");
			break;
		case 1:
			dupchar = strtok(dupchar, " \n");
			if(-1 ==(in = open(dupchar, O_RDWR, S_IRUSR|S_IWUSR)))
				perror("open error");
				if(-1 ==dup2(in, 0)){
					perror("dup error");
					return 1;
				}
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if (-1==close(in))
					perror("close error");
			break;
		case 2:
				dupchar = strtok(dupchar, " \n");
				if (-1 == (out = open(dupchar, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)))
					perror("open error");
				if (-1 ==dup2(out, 1)){
					perror("dup error");
					return 1;
				}
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if (-1 == close(out))
					perror("close error");
			break;
		case 3:
				dupchar = strtok(dupchar, " \n");
				if (-1 == (out = open(dupchar, O_RDWR|O_APPEND, S_IRUSR|S_IWUSR)))
					perror("open error");
				if(-1 ==dup2(out, 1)){
					perror("dup error");
					return 1;
				}
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if(-1 == close(out))
					perror("close error");
			break;
		case 4:
				int t;
				t = strlen(dupchar);
				strncpy(buf, dupchar,t);
				if (-1 == (in  = creat("temp.txt", S_IRUSR|S_IWUSR)))
					perror("cadsfasdf");
				if (-1 == write(in, buf, t))
					perror("adadsf");
				if (-1 ==close(in))
					perror("close error");
				if (-1 ==(in = open("temp.txt", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)))
					perror("asdfads");
				if (-1 == dup2(in, 0))
					perror("asdfasdf");
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if (-1 == close(in))
					perror("adfasd");
			break;
		case 5:
				dupchar = strtok(dupchar, " \n");
				if (-1 == (out = open(dupchar, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)))
					perror("open error");
				if(-1 == dup2(out, 1)){
					perror("open error");
					return 1;
				}
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if (-1 == close(out))
					perror("asdfasdf");
			break;
		case 6:
			dupchar = strtok(dupchar, " \n");
			if (-1 == (out = open(dupchar, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)))
				perror("open error");
			if (-1 == dup2(out, 2)){				
				perror("dup error");
				return 1;
			}
			if (execvp(args[0], args) != 0)
				perror("Command not found");
			if (-1 == close(out))
				perror("adfadsf");
			break;
		case 7:
				dupchar = strtok(dupchar, " \n");
				if (-1 == (out = open(dupchar, O_RDWR|O_APPEND, S_IRUSR|S_IWUSR)))
					perror("open error");
				if(-1 == dup2(out, 2)){
					perror("open error");
					return 1;
				}
				if (execvp(args[0], args) != 0)
					perror("Command not found");
				if (-1 == close(out))
					perror("asdfasdf");
			break;
		case 8:
			dupchar = strtok(dupchar, " \n");
			if (-1 == (out = open(dupchar, O_RDWR|O_APPEND, S_IRUSR|S_IWUSR)))
				perror("open error");
			if (-1 == dup2(out, 1)){				
				perror("dup error");
				return 1;
			}
			if (execvp(args[0], args) != 0)
				perror("Command not found");
			if (-1 == close(out))
				perror("adfadsf");
			break;
	}

	printf("test");

	return 0;
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
	strncpy(e,d + (a - cut), b-a);  //get operator from input
	e[b-a] = '\0';
	if (strstr(e, ";") !=NULL) return 0; 
	else if (strstr(e,"||")!= NULL) return 1;
	else if (strstr(e,"|")!= NULL) return 4;
	else if (strstr(e,"&&")!= NULL) return 2;
	else return 3;
	}
	return 0;
}


int run_pipe(char cmd[64][1024],int i, int pp[2]) // execute command
{
	char * execu= cmd[i];
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	int fd[2];
	int m = flag[i];
	int n;
	if (i!=0)
		n = flag[i-1];
	else 
		n = 0;
	if (flag[i] == 4){
		if (pipe(fd) == -1)
			perror("pipe error");
	}
	char * dupchar;
	char *save;
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
	if (((flag == 4||flag == 1)&&(n != 4))||(flag == 0)||(((flag == 2)||(flag == 3)||(flag == 5)||(flag == 6))&&(m!= 4)))
	{
	execu = strtok(execu, "<12>");
	dupchar = strtok(NULL, "<12>");
	dupchar = strtok(dupchar, " \n");
	execu = strtok_r(execu, " \n", &save);
    int a = 0;
	clean(args);  //clean arguments 
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
			if(i != 0){
				if (-1 == dup2(pp[PIPE_READ], 0))
					perror("1");
		    }
			if(m == 4){
				if(-1==dup2(fd[PIPE_WRITE],1))
					perror("2");
			}
			execute(args, flag, dupchar);
			exit(1);											
		}
		else if(pid > 0)
		{
			int y;
			if ((y = wait(0)) == -1)
				perror("wait error");
			if(m == 4){
				if (-1 ==close(fd[PIPE_WRITE]))
					perror("3");
			}
			if(i != 0){
				if (-1 == close(pp[PIPE_READ]))
					perror("4");
		    }
			i++;
			if(i != num)
				run_pipe(cmd,i,fd);
		}
	}
	}
	else printf("Invalid IO redirection!\n");
	return 0;
}


