#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>
//char cwd[1024];
char cbuf[100];

int main(int argc, char *argv[], char *envp[]);
void exit(int status);

void *malloc(size_t size);
void free(void *ptr);

#endif
