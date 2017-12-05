#include <stdio.h>
#include <syscall.h>
#include <string.h>

int puts(char *s)
{

if(write(1,(uint64_t)s,(uint64_t)strlen(s))==1)
{
	return 1;
}

else
	return 0;
/*
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
*/
}
