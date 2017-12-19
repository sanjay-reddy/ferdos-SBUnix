#include <stdlib.h>
#include <syscall.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[], char *envp[])
{
        int flag = 1;
        char temp[1024];
        if(argc>=2)
                strcpy(temp,argv[1]);
        else
                strcpy(temp," ");
	strcpy(temp,argv[0]);
        echofiles(temp,flag);
        return 0;

}
