#include <stdio.h>
#include <syscall.h>
#include <string.h>
#include <unistd.h>



char * gets(char *s)
{

	read(0,(char *)s,1);
	return s;
	
}
