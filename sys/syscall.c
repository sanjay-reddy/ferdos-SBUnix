#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/interrupts.h>
#include <sys/phymap.h>
#include <sys/task.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
void syscall_handler(struct isr_regs *regs){

 uint64_t no_returned;
 uint64_t return_value;

	// This has the value of the syscal number
        no_returned  = regs->rax;
	
	return_value = syscall_caller(no_returned,regs->rdi,regs->rsi,regs->rdx);
	
	// return value in rax
        
	regs->rax = return_value;

}

uint64_t syscall_caller(uint64_t no_returned,uint64_t par1,uint64_t par2,uint64_t par3){

        switch (no_returned) {

        case SYS_open:
                return sys_open((char *)par1,(uint64_t)par2);
        case SYS_read:
                return sys_read((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_write:
                return sys_write((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
	case SYS_close:
                sys_close((uint64_t)par1);
                return 0;
	case SYS_listfiles:
		return sys_listfiles((char *)par1,(uint64_t)par2);
        case SYS_catfiles:
                return sys_catfiles((char *)par1,(uint64_t)par2);
        case SYS_echofiles:
                return sys_echofiles((char *)par1,(uint64_t)par2);
        case SYS_chdir:
                return sys_chdir((char *)par1);
        case SYS_getcwd:
                return sys_getcwd((char *)par1,(uint64_t)par2);
        case SYS_fork:
              return sys_fork(); 
        case SYS_execve:
              return sys_execve((char *)par1,(char **)par2,(char **)par3);
        case SYS_wait4:
                return sys_waitpid((uint64_t) par1,(uint64_t) par2,(uint64_t) par3);
/*        case SYS_getpid:
                return sys_getpid();
        case SYS_getppid:
                return sys_getppid();
*/
        case SYS_exit:
                sys_exit();
                return 0;
	case SYS_changedir:
		sys_changedir((char *)par1);
		return 0;
	case SYS_listprocess:
		return sys_listprocess();

	case SYS_killprocess:
		return sys_killprocess((uint64_t)par1);
/*        case SYS_nanosleep:
                return sys_sleep((uint64_t)par1);
*/
	default:
                return -1;
//yield();
}

yield();
}

int sys_fork()
{
	int i = proc_fork();
//	yield();
	return i;

}


void sys_exit()
{

	syss_exit();

}

int sys_write(uint64_t fd_count, uint64_t addr, int len)
{
        len = 0;
       // vma_struct *vma_ptr;
        if (fd_count == 0 || fd_count == 1) {
              	kprintf((char *)addr);
                return len;
        }

        return 0;


}

int sys_read(uint64_t par1,uint64_t par2,uint64_t par3)
{
	
	int len = -1;
	if(par1 == 0)
	{
		len = scanf((void *)par2,par3);
	}
	if(par1>2)
	{	
		len =readfile(par1,par3,par2);
	}
	return len;
	//return 0;

}

int sys_open(char * par1, uint64_t par2)
{
	int fno;
	fno = openfile(par1);
	return fno;

}

int sys_echofiles(char * arg, int pipe)
{
	
	kprintf("%s\n",arg);
	return 0;
}


int sys_sleep(uint64_t seconds)
{
//	nano_sleep(seconds);
//	To implement a sleep for the process upon which it is called

	return 0;

}

int sys_listprocess()
{
	proc_ps();
	return 0;
}

int sys_killprocess(uint64_t pid)
{
	killprocess(pid);
	// to implement the kill -9 here
	return 0;
}

int sys_listfiles(char *path,int pipe)
{
//	kprintf("sys %s",path);

	readdirect(path);
	return 0;
}


int sys_catfiles(char *path,int pipe)
{
	memset(filebuf,0,2048);
	uint64_t fno = 0;
	fno = openfile(path);
	//memset(path,0,strlen(path));
	if(fno!=0)
	{
	readfile(fno,2048,(uint64_t)filebuf);
	kprintf("%s\b\n",filebuf);
	}
	return 0;
}

int sys_getpid()
{
	//implement getpid	
	return 0;
}

int sys_getppid()
{
	//implement getppid
	return 0;
}


int sys_waitpid(uint64_t childpid,uint64_t status,uint64_t options)
{
	SYSS_waitpid(childpid);
	return 0;
	
}

void  sys_close(int fd_count)
{	


	//implement sysclose

}


uint64_t sys_getcwd(char *buf, uint64_t size)
{
	strcpy(buf,SYSS_getcwd());
	//kprintf("%s",buf);
	// implement sys_getcwd
        return (uint64_t)buf;
}

int sys_chdir(char *path)
{
	
	int fno = opendirect(path);
	return fno;
}

void sys_changedir(char * path)
{

	SYSS_changedir(path);

}


int sys_execve(char *par1,char **par2,char **par3)
{
	int ret = sanexecve((char *)par1,(char **)par2,(char **)par3);
	return ret;

}
