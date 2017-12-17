#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<syscall.h>
#include<string.h>
#include<sys/tarfs.h>
char arglist[10][64];
char ipCommand[1024];
char s_cwd[128];
char tempcwd[1024];
//char cwd[1024]="bin/";
char PS1[1024];
char commands[10][1000];
int n,sp;
char tempecho[1024];
char tempcat[1024];
char temppath[1024];
char shellbuf[2048];
char shellcommands[20][1000];
void clearip()
{
	memset(tempcat,0,1024);
	memset(ipCommand,0,1024);
	memset(PS1,0,1024);
	for (int i =0; i<10; i++)
	{
		memset(commands[i],0,1000);
		memset(arglist[i],0,64);
	}
	for( int i = 0; i < 20 ; i++)
	{
		memset(shellcommands[i],0,1000);
	}
	memset(s_cwd,'\0',128);
}

void shellparser()
{

	sp = 0;
	for(int si=0;si<strlen(arglist[1]);si++)
   	 {
        	int sj=si;
        	int sk=0;
        	while(shellbuf[sj]!='\n')
        	{
            		shellcommands[sp][sk++]=shellbuf[sj];
            		//printf("%s",ipCommand);
            		//printf("\ni-%c",ipCommand[j]);
            		//printf("\nc-%c",commands[n][k-1]);
            		sj++;
        	}
        	shellcommands[sp][sk]='\0';
        	sp++;
        	si=sj;
    	}
}

int cd(char * path)

{

     	//memset(tempcwd,'\0',1024);
        //memset(s_cwd,'\0',128);
	//char t[100];
	//char cw[100];
	//strcpy(t,path);
	//strcpy();
	//int q=0;
	char cdcwd[128];
	char tempcwd[128];
	getcwd(cdcwd,128);
	//printf("%s",cdcwd);
	if(strcmp(path,"\0")==0 || strcmp(path,"..")==0 )
	{
		changedir("");
		//memset(cwd,0,1024);	
	}
	else
	
	{

		strcpy(tempcwd,cdcwd);	
		strcat(tempcwd,path);
		//strcat(tempcwd,"/");
		//printf("tempcd:%s",tempcwd);
		//chdir(tempcwd);
		int i = chdir(tempcwd);
		if(i != 0 )
		{
			changedir(tempcwd);
			//strcpy(cwd,tempcwd);
			//memset(tempcwd,0,1024);
		}
		
	}
	//memset(tempcwd,'\0',1024);
	//memset(cdcwd,'\0',128);	
	//printf("directory changed to %s\n",cwd);
	return 0;
}


int executeCommand()
{
	if(strcmp(arglist[0],"cd")==0)
	{
		//printf("%s",arglist[1]);
		cd(arglist[1]);
		return 0;
	}
	if(strcmp(arglist[0],"ls")==0 || strcmp(arglist[0],"cat")==0 || strcmp(arglist[0],"echo")==0 || strcmp(arglist[0],"sh")==0 || strcmp(arglist[0],"ps")==0 || strcmp(arglist[0],"kill")==0)
	{
	uint64_t pid = fork();
	if ( pid != 0)
	//if(1==0)
	{
		waitpid(pid,NULL,0);
    	}
	else
	{
		if(strcmp(arglist[0],"ls")==0)
		{
			execve("bin/ls",(char * const *)arglist,NULL);
			//listfiles(cwd,1);
		}	
		if(strcmp(arglist[0],"echo")==0)
		{
			if(strlen(ipCommand)>4)
			{
				memset(tempecho,0,1024);
				int m=0;
				while(m<strlen(ipCommand)-4)
				{
					tempecho[m]=ipCommand[m+4];
					m++;
				}
				tempecho[m]='\0';
				strcpy(arglist[0],tempecho);
				execve("bin/echo",(char * const *)arglist,NULL);
			
			}
		}
		if(strcmp(arglist[0],"cat")==0)
		{
		/*
		char *temp = "etc/test.txt";
		int fd = open(temp,1);
		char tempread[1024];		
		read(fd,tempread,1024);
		printf("file:%s",tempread);
		*/

		execve("bin/cat",(char * const *)arglist,NULL);
		
		/*	strcpy(temppath,arglist[1]);
			strcat(tempcat,cwd);
			//printf("c-%s",temppath);
			strcat(tempcat,temppath);
			catfiles(tempcat,1);
			memset(temppath,0,1024);
		*/
		}
		if(strcmp(arglist[0],"ps")==0)
		{
			execve("bin/ps",(char * const *)arglist,NULL);
		}

		if(strcmp(arglist[0],"kill")==0)
		{
			if(strcmp(arglist[1],"-9")==0)
			{
				//int t = str_to_int(arglist[2]);
				//printf("%d",t);
				//kill(t);
				execve("bin/kill",(char * const *)arglist,NULL);
			}
		}
		
		if(strcmp(arglist[0],"sh")==0)
                {
			shellparser();
			for(int d = 0 ; d < sp ; d++)
			{
				printf("%s,",shellbuf[d]);
			}
                }
		else
		{
			execve(arglist[0],(char * const *)arglist,NULL);
		}

	}
	}
	return 0;
}

int performCommand()
{
    n=0;
    for(int i=0;i<strlen(ipCommand);i++)
    {   
        int j=i;
        int k=0;
        while(ipCommand[j]!='\0' && ipCommand[j]!='\n' && ipCommand[j]!=' ')
        {
            commands[n][k++]=ipCommand[j];
            //printf("%s",ipCommand);
            //printf("\ni-%c",ipCommand[j]);
	    //printf("\nc-%c",commands[n][k-1]);
            j++;
        }    

        commands[n][k]='\0';
	n++;
        i=j;
    }
/*    	for(int i = 0; i < n; i++)
	{
		printf("%s-",commands[i]);
	}
  */ 

    for(int i=0;i<n;i++)
    {
	memset(arglist[i],0,100);
        if(strcmp(commands[i],"/bin/ls")==0 || strcmp(commands[i],"ls")==0)
        {
            strcpy(arglist[i],"ls");
        }
        else if(strcmp(commands[i],"/bin/cat")==0 || strcmp(commands[i],"cat")==0)
        {
            strcpy(arglist[i],"cat");
        }
        else if(strcmp(commands[i],"/bin/echo")==0 || strcmp(commands[i],"echo")==0)
        {
            strcpy(arglist[i],"echo");
        }
	else if(strcmp(commands[i],"/bin/ps")==0 || strcmp(commands[i],"ps")==0)
	{
	    strcpy(arglist[i],"ps");
	}
	else if(strcmp(commands[i],"/bin/kill")==0 || strcmp(commands[i],"kill")==0)
        {
            strcpy(arglist[i],"kill");
        }
	else 
        {
            strcpy(arglist[i],commands[i]);
        }
	//printf("%s ",arglist[i]);
    }

	//for(int m = 0 ; m < n ; m++)
	//	printf("%s-sb ",arglist[m]);
	
	// execute command goes here
	executeCommand();
    return 0;
}


int main(int argc, char *argv[], char *envp[])
{
	/*//int pid, temp = 2;

	printf("Hello");
	
	pid = fork();
	if (pid == 0) {
		printf("In child\n");
		
		temp++;
		//printf("Temp: %d\n", temp);
	} else {
		printf("In parent\n");	
		//printf("Temp: %d\n", temp);
		
	}
	//printf("Temp: %d\n", temp);
while(1);*/
	char m_cwd[128];
	printf("Running SBUSH......\n");	
    	//listfiles("bin/",1);
	//strcpy(cwd,"bin/");
	do{	
		clearip();   
            	//strcpy(PS1,getcwd(s_cwd,128));
		getcwd(m_cwd,128);
		if(strlen(m_cwd)==0)
			printf("~");
		printf("%s>",m_cwd);
       		gets(ipCommand);
        	//printf("%s,%d",ipCommand,strlen(ipCommand));
		performCommand();
        	}while(strcmp (ipCommand, "exit")!=0);
    		printf("\nExiting SBUSH... Stuck on While loop");
		while(1);

	return 0;

}





/*
int main(int argc, char *argv[], char *envp[]) {
//puts("In Main");
char * welcome = "Welcome to FERDOS!";
char *temp = "child";
char *temp1 = "parent";
*/

/*
read(0,ipbuffer,1);
puts("\nRead info :");
printf("%p - %s - %d\n",ipbuffer,ipbuffer,strlen(ipbuffer));
*/

//int pid;
//if(temp == NULL) return 0;
//write(1,(char *)temp,5);
//syscall_3((uint64_t)1, (uint64_t)1,(uint64_t)welcome,(uint64_t)18);
//syscall_3((uint64_t)1, (uint64_t)1,(uint64_t)temp,(uint64_t)5);
//int num = 1927;
//char c = 'v';

// directory check
/*
char *direct="lib/";
printf("\n");
uint64_t dno = chdir(direct);
printf("%p\n",dno);
listfiles(direct,1);
*/

/*
//File Check
char fbuf[15];
char *file = "etc/test.txt";
uint64_t fno = open(file,1);
printf("\nfno:%p",fno);
read(fno,fbuf,10);
printf("\nFile:%s",fbuf);

puts("\nfunc\n");

//printf("yellow\n");
//printf("%d \n %c",num,c);
//while(1);

int pid = fork();

if(pid != 0) syscall_3((uint64_t)1, (uint64_t)1,(uint64_t)temp1,(uint64_t)5);
else syscall_3((uint64_t)1, (uint64_t)1,(uint64_t)temp,(uint64_t)5);
//read(0,buf,5);
//puts(buf);
while(1);
  return 0;
}

*/
