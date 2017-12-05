#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

#define T_SYSCALL   128

//-----------------------------------------Generic Syscalls-----------------------------------//

static __inline uint64_t syscall_0(uint64_t n) {

	uint64_t a = -1;  
    __asm volatile("int $0x80" : "=a" (a) : "0" (n));  
    return a;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
	uint64_t val;                                                                                                                          
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rbx;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1)
                   :"rbp","rcx","rdx","rsi","rdi","r12","r11","r10","r9","r8","r13","r14","r15"//Clobberred registers
                  );
   return val;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	uint64_t res;
    __asm__ volatile ("int %1"\
                    :"=a"(res)\
                    :"i"(T_SYSCALL),"0"(n) ,"b"((uint64_t)(a1)),"c"((uint64_t)(a2))\
                    :"cc","memory");
    return res;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t a; 
    __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3)); 
    return a;
}

//------------------------------------Syscalls wrappers which sbush or any other user process calls-----------------------------//

static __inline uint64_t write(int arg1,uint64_t arg2, uint64_t arg3) {
  uint64_t res;
        if (arg1 < 0) {
                return 0;
        }

        res = (uint64_t)syscall_3((uint64_t)1,(uint64_t)arg1,(uint64_t)arg2,(uint64_t)arg3);

  return res;
}


static __inline char* getcwd(char *buf,	size_t size)
{
	char *cbuf; 
	cbuf = (char *) syscall_2(SYS_getcwd,(uint64_t)buf,(uint64_t)size);
	return cbuf;
}


static __inline int chdir(char *path)
{
	int res;
	res = (int)syscall_1(SYS_chdir,(uint64_t)path);
	return res;
}

static __inline int open(char *pathname,int flags)
{
	int res;
	res = (int)syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);
	return res;
}

static __inline ssize_t read(int fd, void *buf, size_t count) {

        ssize_t res;
	res= (ssize_t)syscall_3(SYS_read,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);
        return res;
}

static __inline int close(int fd)
{
	int res;
	res=(int)syscall_1(SYS_close,(uint64_t)fd);
	return res;
}

static __inline int listfiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_listfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

static __inline int catfiles(char *path,int pipe){
	//if (path != NULL)
	//printf("%s flag %d",path,pipe);

        int ret;
        ret = (int) syscall_2(SYS_catfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

static __inline int echofiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_echofiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

static __inline pid_t fork(void)
{
  	pid_t return_code;
	return_code = (pid_t) syscall_0(SYS_fork);
	return return_code;	
}

static __inline pid_t getpid(void)
{
	pid_t pid;
	pid = (pid_t) syscall_0(SYS_getpid);
	return pid;
}

static __inline pid_t getppid(void)
{
	pid_t ppid;
	ppid = (pid_t) syscall_0(SYS_getppid);
	return ppid;
}

static __inline int execve(const char *filename,char *const argv[],char *const envp[])
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

static __inline int execvpe(const char *filename,char *const argv[],char *const envp[])
{        
        const char *temp = filename;
	while ( temp && !((*temp == 'b') && (*(temp+1) == 'i') && (*(temp+2) == 'n'))){
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

static __inline pid_t waitpid(pid_t pid,int *status,int options)
{
	pid_t retPid;
	retPid = (pid_t)syscall_3(SYS_wait4,(uint64_t)pid,(uint64_t)status,(uint64_t)options);

	return retPid;
}

static __inline unsigned int sleep(unsigned int seconds)
{
	unsigned int ret;
	unsigned int ticks = seconds*1000;
	ret =(unsigned int)syscall_1(SYS_nanosleep,(uint64_t)ticks);
	return ret;
}

static __inline int kill(int pid_t)
{
	int ret;
	ret = (int)syscall_1(SYS_killprocess,(uint64_t)pid_t);
	return ret;
}


/*
uint64_t write(int arg1,const void *arg2, uint64_t arg3) {
  ssize_t res;
        if (arg1 < 0) {
                return 0;
        }

	res = (ssize_t )syscall_3(SYS_write,(uint64_t)arg1,(uint64_t)arg2,(uint64_t)arg3);

  return res;
}
*/

#endif
