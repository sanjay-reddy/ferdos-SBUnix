#include "sys/task.h"
#include "sys/phymap.h"
#include "sys/kprintf.h"
#include "sys/gdt.h" 
#include "syscall.h"
#include "sys/syscall.h"
static Task *runningTask;
static Task mainTask;
static Task otherTask;

char test[5] = {"test"};

//tss_entry_t tss_entry; 
static void testUserFunction()
{

        kprintf("testUserFunction\n");
while(1);
}

static void otherMain() {
//    kprintf("Hello multitasking world!"); // Not implemented here...
jump_to_user_mode();
//syscall_3(SYS_write,(uint64_t)1,(uint64_t)test,(uint64_t)5);
//    yield();
}
 
void initTasking() {
    // Get EFLAGS and CR3
    __asm__ __volatile__("movq %%cr3, %%rax; movq %%rax, %0;":"=g"(mainTask.regs.cr3)::"memory");
    __asm__ __volatile__("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=g"(mainTask.regs.rflags)::"memory");
 
    createTask(&otherTask, otherMain, mainTask.regs.rflags, (uint64_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;
 
    runningTask = &mainTask;
}
 
void createTask(Task *task, void (*otherMain)(), uint64_t flags, uint64_t *pagedir) {
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.rflags = flags;
    task->regs.rip = (uint64_t) otherMain;
    task->regs.cr3 = (uint64_t) pagedir;
    task->regs.rsp = (uint64_t) (page_alloc()->addr + 0xFA0); // Not implemented here
    task->next = 0;
    uint64_t * valPtr = (uint64_t *)(task->regs.rsp + 0x30);
    *valPtr = task->regs.rip;
}
 
void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs,&runningTask->regs);
}

void switchTask(Registers *from, Registers *to)

{
    __asm__ __volatile__("pushq %rax");
    __asm__ __volatile__("pushq %rbx");
    __asm__ __volatile__("pushq %rcx");
    __asm__ __volatile__("pushq %rdx");
    __asm__ __volatile__("pushq %rsi");
    __asm__ __volatile__("pushq %rdi");
    __asm__ __volatile__("movq %%rsp, %0":"=g"(from->rsp)::"memory");
    __asm__ __volatile__("movq %0, %%rsp"::"m"(to->rsp));
    __asm__ __volatile__("popq %rdi");
    __asm__ __volatile__("popq %rsi");
    __asm__ __volatile__("popq %rdx");
    __asm__ __volatile__("popq %rcx");
    __asm__ __volatile__("popq %rbx");
    __asm__ __volatile__("popq %rax");
    __asm__ __volatile__("retq");

}

Task * create_process(char *file_name)
{
	Task *new_proc = (Task *)kmalloc(sizeof(Task));
        
        new_proc->regs.cr3 = (uint64_t)set_user_AddrSpace();
        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();
        
        set_CR3((struct PML4 *)new_proc->regs.cr3);
        
        elf_load(new_proc,file_name);
        
        set_CR3((struct PML4 *)curr_CR3);
        return new_proc;
}

void jump_to_user_mode()
{
uint64_t funcadd = (uint64_t)testUserFunction;
uint64_t * rsp;
uint64_t u_rsp = (page_alloc()->addr +(uint64_t)0xFA0);


//__asm__ __volatile__("cli");

__asm__ __volatile__("movq %%rsp,%0;":"=r"(rsp)::"memory");
set_tss_rsp(rsp);

__asm__ __volatile__("movq %0,%%rax;": : "r"(u_rsp));
__asm__ __volatile__("pushq $0x23");
__asm__ __volatile__("pushq %rax");
__asm__ __volatile__("pushfq");
/*__asm__ __volatile__("popq %rax");
__asm__ __volatile__("orq $0x200,%rax");
__asm__ __volatile__("pushq %rax");
*/
__asm__ __volatile__("pushq $0x2B");
__asm__ __volatile__("pushq %0"::"r"(funcadd):"memory");
//__asm__ __volatile__("sti");
__asm__ __volatile__("iretq");
set_tss_rsp(rsp);

}