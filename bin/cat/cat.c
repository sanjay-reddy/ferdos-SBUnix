#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<syscall.h>
#include<dirent.h>
#include<sys/defs.h>
#include<string.h>

int main(int argc, char *argv[], char *envp[])
{
	char cat_cwd[128];
        getcwd(cat_cwd,128);
	strcat(cat_cwd,argv[1]);
        //char path[1024];
        //strcpy(path,argv[1]);
        char fileContent[1024];
        //int fd = open(path, 1);
        int fd; 
	fd = open(cat_cwd, 1);
	/*if(fd < 0) {
                puts("Cannot open file!");
                return -1;
        }
	*/
        //ssize_t size = 0;
        read(fd,fileContent,1024);
        printf("%s",fileContent);
        //close(fd);
	
	//catfiles("etc/test.txt",1);
//        while(1);
	return 0;
}
