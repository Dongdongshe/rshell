# rshell
This is a simple shell program using c++. It implements many syscalls like 
fork(), execp() to execute commands from user input. For example, if you 
input "ls -a -l", it will execute this command and return results to user.

#Features
1.It can print user name and host name as a prompt.
2.It can detect comments following by #
3.It can receive serverl commands connected by operator(; || &&).
4.It has a inner exit function.
*prompt like user@host>>.
*example command  ls -a#show current files in this directory.
*example command like ls||pwd&&ps;exit 
*command exit can exit this program

#Prerequisites
Required OS: Linux 
Required software: G++
Testing environment: Ubuntu 14.04 X64

#How to install
1.Go to root directory of project.
2.Run make command.
3.Run bin/rshell

#Bugs needed to fix
1.When running command script then run rshell, the user name will become NULL, while in normall situation, this will not happen.
2.Operator like |*|, &**& will be detected as ;.

#Things not to do
Pipes and cd command will be added in future version.






