#include <stdio.h>
#include <syscall.h>
/*
int putchar(int c)
{
	// write character to stdout
	if (write(1, &c, sizeof(char)) == 1)
		return c;
	else
		return EOF;
}
*/

int putchar(int c)
{
  // write character to stdout
  return c;
}

