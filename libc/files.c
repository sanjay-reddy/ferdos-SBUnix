#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>
#include<unistd.h>
#include<string.h>

ssize_t write(int arg1,char * arg2, uint64_t arg3) {
  ssize_t res;
        if (arg1 < 0) {
                return 0;
        }

        res = (ssize_t)syscall_3((uint64_t)SYS_write,(uint64_t)arg1,(uint64_t)arg2,(uint64_t)arg3);

  return res;
}

ssize_t read(int fd, void *buf, size_t count) {

        ssize_t res;
        res= (ssize_t)syscall_3(SYS_read,(uint64_t)fd,(uint64_t)buf,count);
        return res;
}

int open(char *pathname,int flags)
{
        int res;
        res = (int)syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);
        return res;
}


int close(int fd)
{
        int res;
        res=(int)syscall_1(SYS_close,(uint64_t)fd);
        return res;
}

int listfiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_listfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

int catfiles(char *path,int pipe){
        //if (path != NULL)
        //printf("%s flag %d",path,pipe);

        int ret;
        ret = (int) syscall_2(SYS_catfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}


int echofiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_echofiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

void ps()
{
	
	syscall_0(SYS_listprocess);

}

/*char* getcwd()
{
	char *gc;	
	gc = (char*)syscall_2(SYS_getcwd,(uint64_t)gc,(uint64_t)100);
	return gc;
}*/
char* getcwd(char *buf,size_t size)
{
	char *cbuf; 
	cbuf = (char *) syscall_2(SYS_getcwd,(uint64_t)buf,(uint64_t)size);

	return cbuf;
}

int chdir(char *path)
{
	int res;
	res = (int)syscall_1(SYS_chdir,(uint64_t)path);
	return res;
}

pid_t fork(void)
{
	pid_t return_code;
	return_code = (pid_t) syscall_0(SYS_fork);
	return return_code;
}

pid_t getpid(void)
{
	pid_t pid;
	pid = (pid_t) syscall_0(SYS_getpid);
	return pid;
}

pid_t getppid(void)
{
	pid_t ppid;
	ppid = (pid_t) syscall_0(SYS_getppid);
	return ppid;
}



pid_t waitpid(pid_t pid,int *status,int options)
{
	pid_t retPid;
	retPid = (pid_t)syscall_3(SYS_wait4,(uint64_t)pid,(uint64_t)status,(uint64_t)options);

        return retPid;
}

int execve(const char *filename,char *const argv[],char *const envp[])
{

        const char *temp = filename;
        while (temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){

                temp++;
        }
        if ((*temp == '\0') || (*(temp+4) == '\0')) {
                return -1;
        }

        int return_code;
        return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

        if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
        return return_code;
}

int execvpe(const char *filename,char *const argv[],char *const envp[])
{
        const char *temp = filename;
       /* while ( temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
                temp++;
        }
        if ((*temp == '\0') || (*(temp+4) == '\0')) {
                return -1;
        }
	*/
        int return_code;

        return_code = (int)syscall_3(SYS_execve,(uint64_t)temp,(uint64_t)argv,(uint64_t)envp);

        if (-125 <= return_code && return_code < 0) {
                return_code = -1;
        }
        return return_code;
}


unsigned int sleep(unsigned int seconds)
{
        unsigned int ret;
        unsigned int ticks = seconds*1000;
        ret =(unsigned int)syscall_1(SYS_nanosleep,(uint64_t)ticks);
        return ret;
}


int kill(int pid)
{
        int ret;
        ret = (int)syscall_1(SYS_killprocess,(uint64_t)pid);
        return ret;
}

void exit(int arg1)
{
        syscall_0(SYS_exit);
}
void changedir(char *path)
{
        
        syscall_1(SYS_changedir,(uint64_t)path);
}
