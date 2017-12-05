#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>

int execve(const char *filename,char *const argv[],char *const envp[])
{      

	const char *temp = filename;
	while (temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
		temp++;
	} 
        if (*temp == NULL || (*temp == '\0') || (*(temp+4) == '\0')) {
                printf("invalid binary path \n");
		return -1;
        }
	int return_code;
	return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

	if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
	return return_code;
}
int
execvpe(
        const char *filename,
        char *const argv[],
        char *const envp[])
{        
        const char *temp = filename;
	while ( temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
                temp++;
        }
	if (*temp == NULL || (*temp == '\0') || (*(temp+4) == '\0')) {
		printf("invalid binary path \n");
		return -1;
	}
	int return_code;

        return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

        if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
        return return_code;
}

