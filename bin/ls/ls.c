#include <stdio.h>
#include <unistd.h>
#include <sys/defs.h>
#include <dirent.h>
#include <syscall.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
	char ls_cwd[128];
	getcwd(ls_cwd,128);
        char temp[100];
        if (argc >= 2){
		//printf("%s",argv[1]);
                strcpy(temp,argv[1]);
        } else {
	//	printf("%s",ls_cwd);
                strcpy(temp,ls_cwd);
        }
        int flag = 1;
	
//	argc =2;
/*	for(int i = 1;i < argc; i++)
		printf("%s",argv[i]);
*/ 
	listfiles(temp,flag);
      // listfiles(temp,flag);
//	while(1);
//exit(0);
        return 0;

}
/*
    char pwd[1024];
    DIR *curdir;
    struct dirent *file;
    getcwd(pwd, sizeof(pwd));
    curdir = opendir(pwd);
        printf("in ls");
    while((file = readdir(curdir)) != NULL)
    {
        char * fileName = file->d_name;
        if(fileName[0]!='.')
        {
            puts(fileName);
        }
    }
    putchar('\n');
    closedir(curdir);
   return 1;
*/


