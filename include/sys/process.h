#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/interrupts.h>
#include <dirent.h>
#include <sys/kprintf.h>

int execve (const char *filename,char *const argv[],char *const envp[]);

int execvpe(const char *filename,char *const argv[],char *const envp[]);

