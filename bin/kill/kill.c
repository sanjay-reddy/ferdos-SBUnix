//#include <stdlib.h>
#include <sys/defs.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
        int pid;

        if (argc >= 3)
        {
                pid = str_to_int(argv[2]);
        }
        else
        {
                return 0;
        }

        if (pid > 0)
                kill(pid);
	
        return 0;

}
