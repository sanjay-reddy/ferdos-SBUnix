#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H
#include<sys/interrupts.h>
#include<sys/defs.h>
#include <sys/elf64.h>
#include <dirent.h>

#define SYS_exit       60
#define SYS_brk        12
#define SYS_fork       57
#define SYS_getpid     39
#define SYS_getppid   110
#define SYS_execve     59
#define SYS_wait4      61
#define SYS_nanosleep  35
#define SYS_alarm      37
#define SYS_getcwd     79
#define SYS_chdir      80
#define SYS_open        2
#define SYS_read        0
#define SYS_write       1
#define SYS_lseek       8
#define SYS_close       3
#define SYS_pipe       22
#define SYS_dup        32
#define SYS_dup2       33
#define SYS_getdents   78
#define SYS_opendir    4
#define SYS_readdir    5
#define SYS_closedir   6
#define SYS_listprocess 7
#define SYS_killprocess 13
#define SYS_clearscreen 14
#define SYS_shutdown    15 
#define SYS_listfiles   16
#define SYS_catfiles    17
#define SYS_echofiles   18
#define SYS_yield       19
#define SYS_changedir   150
/*
static void syscall_handler(registers_t *regs);

static void *syscalls[3] =
{
   &monitor_write,
   &monitor_write_hex,
   &monitor_write_dec,
};


u32int num_syscalls = 3;

void initialise_syscalls()
{
   // Register our syscall handler.
   register_interrupt_handler (0x80, &syscall_handler);
}

void syscall_handler(registers_t *regs)
{
   // Firstly, check if the requested syscall number is valid.
   // The syscall number is found in EAX.
   if (regs->rax >= num_syscalls)
       return;

   // Get the required syscall location.
   void *location = syscalls[regs->rax];

   // We don't know how many parameters the function wants, so we just
   // push them all onto the stack in the correct order. The function will
   // use all the parameters it wants, and we can pop them all back off afterwards.
   int ret;
   asm volatile (" \ 
     push %1; \ 
     push %2; \ 
     push %3; \ 
     push %4; \ 
     push %5; \ 
     call *%6; \ 
     pop %%rbx; \ 
     pop %%rbx; \ 
     pop %%rbx; \ 
     pop %%rbx; \ 
     pop %%rbx; \ 
   " : "=a" (ret) : "r" (regs->rdi), "r" (regs->rsi), "r" (regs->rdx), "r" (regs->rcx), "r" (regs->rbx), "r" (location));
   regs->rax = ret;
}
*/

void syscall_handler(struct isr_regs *);

uint64_t syscall_caller(uint64_t,uint64_t,uint64_t,uint64_t);

int sys_write(uint64_t,uint64_t,int);

int sys_open(char *,uint64_t);

int sys_read(uint64_t,uint64_t,uint64_t);

int sys_echofiles(char * arg, int pipe);

int sys_sleep(uint64_t seconds);

int sys_killprocess(uint64_t pid);

int sys_listfiles(char *path,int pipe);

int sys_catfiles(char *path,int pipe);

int sys_listprocess();

int sys_getpid();

int sys_getppid();

void  sys_close(int fd_count);

uint64_t sys_getcwd(char *buf, uint64_t size);

int sys_chdir(char *path);

int sys_fork();

int sys_execve(char *par1,char **par2,char **par3);

void sys_exit();

void sys_changedir(char *path);

int sys_waitpid(uint64_t childpid,uint64_t status,uint64_t options);

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
//typedef void (*isr_t)(registers_t);
//void register_interrupt_handler(u8int n, isr_t handler);

#endif
