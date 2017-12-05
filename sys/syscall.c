#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/interrupts.h>
#include <sys/phymap.h>
#include <sys/task.h>
#include <sys/syscall.h>

void syscall_handler(struct isr_regs *regs){

 uint64_t no_returned;
 uint64_t return_value;

        no_returned  = regs->rax;
        return_value = syscall_caller(no_returned,regs->rdi,regs->rsi,regs->rdx);
        regs->rax = return_value;// return value in rax

	//	*((int64_t *)current->init_kern - 9) = return_value;	
}

uint64_t syscall_caller(uint64_t no_returned,uint64_t par1,uint64_t par2,uint64_t par3){

        switch (no_returned) {

        case SYS_open:
                return sys_open((char *)par1,(uint64_t)par2);
        case SYS_read:
                return sys_read((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_write:
                return sys_write((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        default:
                return -1;

}

}

int sys_write(uint64_t fd_count, uint64_t addr, int len)
{
       // int len = 0;
       // vma_struct *vma_ptr;
        if (fd_count == 0 || fd_count == 1) {
              	kprintf((char *)addr);
                return len;
        }

        return -1;

}

int sys_read(uint64_t par1,uint64_t par2,uint64_t par3)
{

	return -1;

}

int sys_open(char * par1, uint64_t par2)
{

	return -1;

}


