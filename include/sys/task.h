#ifndef __TASK_H__
#define __TASK_H__
#define USER_STACK_TOP 0xF0000000
#define HEAP_START 0x08000000
#include "sys/defs.h" 
extern void initTasking();
 
typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, rip, rflags, cr3;
} Registers;
 
typedef struct Task {
    Registers regs;
    uint16_t pid;
    uint16_t ppid;
    uint64_t pml4e_addr;	
    struct Task *next;
    uint64_t kstack[512];
    uint64_t ustack;
    uint64_t task_rsp;
    uint64_t task_cr3;
    uint64_t task_rip;
    uint64_t entry_pt;
    uint64_t kernel_rsp;
} Task;
//Task * current;
extern void createTask(Task*, void(*)(), uint64_t, uint64_t*);
void create_init_process(); 
extern void yield(); // Switch task frontend
extern void switchTask(Registers *old, Registers *new); // The function which actually switches

struct file{
    uint64_t   start;       /* start address of region */
    uint64_t   pgoff;       /* offset in file or NULL */
    uint64_t   size;        /* region initialised to here */
    uint64_t   bss_size;
};

typedef struct vm_area_struct {
        uint64_t start;             
        uint64_t end;               
        struct vm_area_struct *next; 
        uint64_t flags;              
        uint64_t type;
	uint64_t vm_memsz;
	uint64_t vm_offset;
	uint64_t vm_file;
        struct file *file;           // reference to file descriptors for file opened for writing
}vma_struct;
vma_struct * head;
// A struct describing a Task State Segment.
struct tss_entry_struct
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // everything below here is unusued now.. 
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         
   uint32_t cs;        
   uint32_t ss;        
   uint32_t ds;        
   uint32_t fs;       
   uint32_t gs;         
   uint32_t ldt;      
   uint16_t trap;
   uint16_t iomap_base;
} __packed;
Task * create_process(char *file_name);
typedef struct tss_entry_struct tss_entry_t;
void jump_to_user_mode();
#endif /* __TASK_H__ */
