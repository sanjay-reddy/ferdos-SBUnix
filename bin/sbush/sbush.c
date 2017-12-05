/*
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *PS1;
//static int backgroundProcess = 0;


/*
char *arglist[10];
char *arglistparam[10];
int executeScipt(char*);
int executeCommand();
int performCommand(char[]);
int executeCDCommand(char*);
int executeScript(char * scriptFile)
{
    char fileContent[100];
    int fp = open(scriptFile, O_RDONLY);
    if (fp == -1) {
      printf("Cannot open file!");
      return -1;
    }
    ssize_t size = 0;
    while ((size = read(fp,fileContent,100)) > 0) {
        {
            char buff[100];
            int i=0;
            int j=0;
            while(i < size)
            {
                if(fileContent[i]=='\n')
                {
                    buff[j]='\0';
                    j=0;
                    performCommand(buff);
                }
                else{
                buff[j++]=fileContent[i];}
                i++;
            }
        }
     }
    close(fp);
    return 1;
}

int executeCommand()
{
    //char* env[]={NULL};
    int status;
    if (fork() != 0)
    {
        if(!backgroundProcess)
        {
                waitpid(-1, &status, 0);
        }
        else
        {
            backgroundProcess=0;
        }
    }
    else
    {
        if(strcmp(arglist[0],"cd")==0)
        {
                executeCDCommand(arglist[1]);
        }
        else if(strcmp(arglist[0],"sh")==0)
        {
            executeScript(arglist[1]);
        }
        else{
        //printf("%s\n",arglistparam);
        execve(arglist[0],arglist,NULL);
    }
    }
    return 0;
}

int performCommand(char ipCommand[1000])
{
//      printf("%s",ipCommand);
    if(strstr(ipCommand,"|")!=NULL)
    {
        arglist[0]="bin/pipe";
        arglist[1]=ipCommand;
        executeCommand();
    }
    else{
    char commands[10][1000];
    int n=0;
    for(int i=0;i<strlen(ipCommand);i++) 
    {
        int j=i;
        int k=0;
        while(ipCommand[j]!='\0' && ipCommand[j]!='\n' && ipCommand[j]!=' ')
        {
            commands[n][k++]=ipCommand[j];
            //printf("%s",ipCommand);
            //printf("%c",commands[n][k-1]);
            j++;
        }
        commands[n][k]='\0';
        n++;
        i=j;
    }
    //printf("\ndsf%s",commands[0]);
    //printf("\nsdfds%s",commands[1]);  
    if(backgroundProcess)
    {
        n=n-1;
    }
    for(int i=0;i<n;i++)
    {
        if(strcmp(commands[i],"/bin/ls")==0 || strcmp(commands[i],"ls")==0)
        {
            arglist[i]="/bin/ls";
        }
        else if(strcmp(commands[i],"/bin/cat")==0 || strcmp(commands[i],"cat")==0)
        {
            arglist[i]="/bin/cat";
        }
        else if(strcmp(commands[i],"/usr/bin/grep")==0 || strcmp(commands[i],"grep")==0)
        {
            arglist[i]="/bin/grep";
        }
        else
        {
            arglist[i]=commands[i];
        }
    }

        executeCommand();
    }
    return 0;
}

int executeCDCommand(char *path)
{
    char temp_path[1024];
    strcpy(temp_path,path);
    char cwd[1024];
    int chdirRet;
    if(path[0]!='/')
    {
        if(getcwd(cwd,sizeof cwd)==NULL)
        {
            puts("Cannot retreive current working directory");
        }
        strcat(cwd,"/");
        strcat(cwd,temp_path);
        chdirRet=chdir(cwd);
    }
    else
    {
        chdirRet=chdir(temp_path);
    }
    if(chdirRet == -1)
    {
        puts("Change dir failed");
    }
    return 0;
}

*/

int main(int argc, char *argv[], char *envp[]) {
    char ipCommand[1000]="exit";

    do{
          //  char ch;
          //  int n=0;
          //  char cwd[200];
  
/*
	if (getcwd(cwd, 200) != NULL){
        PS1=cwd;
   	 }
*/
  
          strcat(PS1,">");
          puts(PS1);
 /*       while((ch=getchar())!='\n')
        {
            ipCommand[n++]=ch;
        }
        if(ipCommand[n-1]=='&')
        {
                backgroundProcess=1;
        }
        ipCommand[n]='\0';
*/   
 //    performCommand(ipCommand);
        }while(strcmp (ipCommand, "exit",strlen(ipCommand))!=0);
    return 0;
}


/*
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
  puts("sbush> ");
  return 0;
}
*/
