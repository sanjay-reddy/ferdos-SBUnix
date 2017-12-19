#include "sys/task.h"
#include "sys/phymap.h"
#include "sys/kprintf.h"
#include "sys/gdt.h" 
#include "syscall.h"
#include "sys/syscall.h"
#include "string.h"
static Task *runningTask;
static Task mainTask;
static Task otherTask;
int processID[100];
static Task *current;
static Task *currentRunningTask;
static Task *from,*to;
char test[5] = {"test"};
extern void isr_syscall();

int killprocess(uint64_t pid)
{
	//implement kill here
	Task *cur;
        cur = current;
       // kprintf("PID----Name----State\n");
        while(cur != NULL)
        {

                //kprintf("%d----%s----%d\n",cur->pid,cur->task_name,(int)cur->state);
                if(cur->pid==pid)
			break;
		cur=cur->next;
        }
        if(cur!=NULL)
		{cur->state = TASK_ENDED;kprintf("Killed: %d\n",cur->pid);}
	proc_ps();
	yield();
	return 0;
}


char * getTaskState(uint64_t state)
{
	switch(state)
	{
		case 0x0:return "TASK_WAITING";
		case 0x1:return "TASK_RUNNING";
		case 0x3:return "ZOMBIE STATE";
		case 0x4:return "WAITING FOR CHILD";
	}
return "Unknown Status";
}
int proc_ps()
{
	Task *cur;
	cur = current;
	kprintf("------------------------------\n");
	kprintf("PID	PPID      Name        State\n");
	kprintf("------------------------------\n");
	while(cur != NULL)
	{
		if(strlen(cur->task_name)!=0 && cur->state != 2 )
		kprintf("%d--%d--%s----%s\n",cur->pid,cur->ppid,cur->task_name,getTaskState(cur->state));
		cur=cur->next;
	}	
	return 0;
}

/* get the available process ID */ 
int getProcessID()
{
	int id = 1;
	for(;id < 100; id++) {
		if(processID[id] == 0) {
			processID[id] = 1;
			return id;
		}
	}
	return -1;
}

/* mark all the 100 entries as unused */
void init_process_map()
{
	int id = 1;
	for(; id < 100; id++)
		processID[id] = 0;
}
//tss_entry_t tss_entry; 
/*static void testUserFunction()
{

        kprintf("testUserFunction\n");
while(1);
}*/

void  moveProcessToEnd(Task * element) {
    Task * list = current;	
    while (list->next != NULL) {
        if (list->next == element)
            list->next = list->next->next;

        list = list->next;
    }

    list->next = element;
    element->next = NULL;
}
void addProcessToList(Task * newTask){

    //checking for first insertion
    if(current->next == NULL){
        current->next = newTask;
    }

    else
    {
        //else loop through the list and find the last and insert it.
        Task *currentPtr = current;
        while (1) { 
            if(currentPtr->next == NULL)
            {
                currentPtr->next = newTask;
                break; 
            }
            currentPtr = currentPtr->next;
        }
    }
}

static void otherMain() {
    //kprintf("Hello multitasking world!"); // Not implemented here...
    //Task *user_proc = create_process("bin/sbush");
     // jump_to_user_mode(user_proc);
while(1){
kprintf("Init Process Invoked!\n");
yield();
}
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


/* creating the first process. */
void create_init_process()
{
	/* init process task struct */
	Task *initial_task = (Task *) kmalloc(sizeof(Task));
	strcpy(initial_task->task_name,"Init Task");
	//initial_task->task_state = TASK_RUNNING;
	initial_task->pid = getProcessID();
	initial_task->ppid = 0;
	initial_task->next = NULL;
	initial_task->task_cr3 = (uint64_t)get_CR3();
	//initial_task->task_rip = (uint64_t)&init_func;

	//first = init;

	/* set the kernel stack */
	//void *kstack = (void *)kmalloc();
	initial_task->kernel_rsp = initial_task->kstack[511];
	initial_task->kstack[511-6]=(uint64_t) otherMain;
	initial_task->state=TASK_WAITING;	
	current = initial_task;
	
	//current->next=NULL;
	//addProcessToList(initial_task);
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

Task * getNextWaitingTask()
{
	Task *nxtTask = current->next;
	while(nxtTask != NULL)
	{
		if(nxtTask->state == TASK_WAITING)
		{
			moveProcessToEnd(nxtTask);
			return nxtTask; 
		}
		nxtTask = nxtTask->next;
		 
	}
	return NULL;
} 

void yield() {
    Task * nextTask = getNextWaitingTask();
    if(nextTask != NULL)
    	{from=current;to=nextTask;switchTask();}
}

void switchTask()

{
	__asm__ __volatile__("addq $8, %rsp");
	currentRunningTask = to;
	currentRunningTask->state = TASK_RUNNING;
	if(from->state != TASK_ENDED)
		from->state = TASK_WAITING;
    set_CR3((struct PML4 *)to->task_cr3);
	__asm__ __volatile__("pushq %rax");
    __asm__ __volatile__("pushq %rbx");
    __asm__ __volatile__("pushq %rcx");
    __asm__ __volatile__("pushq %rdx");
    __asm__ __volatile__("pushq %rsi");
    __asm__ __volatile__("pushq %rdi");
    __asm__ __volatile__("movq %%rsp, %0":"=g"(from->kernel_rsp)::"memory");
    __asm__ __volatile__("movq %0, %%rsp"::"m"(to->kernel_rsp));
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
        new_proc->pid = getProcessID();
        new_proc->task_cr3 = (uint64_t)set_user_AddrSpace();
        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();
        
        set_CR3((struct PML4 *)new_proc->task_cr3);
        
        elf_load(new_proc,file_name,0,NULL);
        new_proc->ppid = current->pid;
        set_CR3((struct PML4 *)curr_CR3);
	new_proc->next = NULL;
	strcpy(new_proc->task_name,file_name);
	strcpy(new_proc->cwd,"bin/");
	addProcessToList(new_proc);
        return new_proc;
}

Task * copyParentToChild(Task * parent)
{
	Task* child = (Task *) kmalloc(sizeof(Task));
	child->pid = getProcessID();
	child->ppid = parent->pid;
	child->next = NULL;
	strcpy(child->cwd,parent->cwd);
//	child->task_rsp=(uint64_t)USER_STACK_TOP;
	// set child page tables 
	child->task_cr3 = (uint64_t)page_alloc()->addr;
	copy_page_tables_forchild(child->task_cr3);
	set_CR3((struct PML4 *)child->task_cr3);
	 //set the child CR3 to allocate memory to its structures
//	set_CR3((struct PML4 *)child->task_cr3);
	// copy vmas 
	vma_struct *parent_vma = parent->vma;
	vma_struct *child_vma = NULL;
	int first = 0;

	while(parent_vma) {

		if(child_vma == NULL)
			first = 1;

		child_vma = (vma_struct *)kmalloc(sizeof(vma_struct));

		memcpy(child_vma, parent_vma, sizeof(vma_struct));

		if(first) {
			child->vma = child_vma;
			first = 0;
		}

		if(child_vma->next)
			child_vma = child_vma->next;
		parent_vma = parent_vma->next;
	}	

	// copying the stack values.
child->kstack[511] = parent->kstack[511-1];
child->kstack[511-1] = parent->kstack[511-2];
child->kstack[511-2] = parent->kstack[511-3];
child->kstack[511-3]  =parent->kstack[511-4];
child->kstack[511-4] = parent->kstack[511-5];

child->kstack[511-5] =parent->kstack[511-6];
child->kstack[511-6] = 0;
child->kstack[511-7] = parent->kstack[511-8];
child->kstack[511-8] = parent->kstack[511-9];
child->kstack[511-9] = parent->kstack[511-10];
child->kstack[511-10] =parent->kstack[511-11];
child->kstack[511-11] = parent->kstack[511-12];
child->kstack[511-12] = parent->kstack[511-13];
child->kstack[511-13] = parent->kstack[511-14];
child->kstack[511-14] =(uint64_t)(0x13+isr_syscall);
child->task_rip=(uint64_t)(0x13+isr_syscall);
//(child->kstack[511-13]) = isr_handler+0x08*9;

child->kernel_rsp=(uint64_t)&child->kstack[511-20];
//switchTask(parent,child);
return child;
}

int proc_fork()
{
	Task * child = (Task *) copyParentToChild(currentRunningTask);
	//set_CR3((struct PML4 *)currentRunningTask->task_cr3);
	addProcessToList(child);
//	uint64_t *stack_addr = (uint64_t *)get_phy_addr(USER_STACK_TOP);
  //      uint64_t *ptr = (uint64_t *)((uint64_t)stack_addr +0x1000 - 16);
//        child->task_rsp = (uint64_t)(ptr);
	set_CR3((struct PML4 *)currentRunningTask->task_cr3);
	from = currentRunningTask;to=child;
	 set_tss_rsp((void *)&child->kernel_rsp);
	switchTask(currentRunningTask,child);
	return child->pid;
}

void jump_to_user_mode(Task *user_process)
{
currentRunningTask = user_process;
currentRunningTask->state = TASK_RUNNING;

//uint64_t funcadd = (uint64_t)userMain;

uint64_t funcadd = user_process->task_rip;

//uint64_t * rsp; 

//u_rsp = (page_alloc() +(uint64_t)0xFA0);

//__asm__ __volatile__("cli");
//__asm__ __volatile__("movq %%rsp,%0;":"=r"(user_process->kernel_rsp)::"memory");

set_tss_rsp((void *)&user_process->kstack[511]);
//__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(user_process->kernel_rsp));

//set_tss_rsp((void *)&user_process->kstack[511]);
__asm__ __volatile__("movq %0,%%cr3;": : "r"(user_process->task_cr3));
__asm__ __volatile__("movq %0,%%rax;": : "r"(user_process->task_rsp));
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

}
int sanexecve(char *binary,char **argv, char **envp)
{
	uint64_t argc = 0;
        int k=0;
        if(argv != NULL) {
                while((char *)(argv+k) != NULL && strcmp((char *)(argv+k),"")!=0){
               //         strcpy(args[argc],argv[k]);
                        argc++;
			k+=8;
                }
        }
	
	currentRunningTask->state=TASK_ENDED;

	Task *new_proc = (Task *)kmalloc(sizeof(Task));
	new_proc->pid = getProcessID();
	new_proc->ppid=currentRunningTask->ppid;
        new_proc->task_cr3 = (uint64_t)set_user_AddrSpace();
        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

        set_CR3((struct PML4 *)new_proc->task_cr3);

        elf_load(new_proc,binary,argc,argv);

        //set_CR3((struct PML4 *)curr_CR3);
        new_proc->next = NULL;
        strcpy(new_proc->task_name,binary);
	strcpy(new_proc->cwd,currentRunningTask->cwd);
	addProcessToList(new_proc);

        //new_proc->task_rsp = (uint64_t)ptr;
	//new_proc->kernel_rsp=(uint64_t)&new_proc->kstack[511];
	set_CR3((struct PML4 *)curr_CR3);	
	//new_proc->kstack[511]=new_proc->task_rip;
	//from=currentRunningTask;to=new_proc;
	//switchTask();
	jump_to_user_mode(new_proc);
	return 0;	
}
void syss_exit()
{
	currentRunningTask->state=TASK_ZOMBIE;
	yield();
}
char * SYSS_getcwd()
{
	//kprintf("kernel:%d",strlen(currentRunningTask->cwd));
	return currentRunningTask->cwd;
}
void SYSS_changedir(char *path)
{
if(strlen(path)==0) strcpy(currentRunningTask->cwd,"");
else strcpy(currentRunningTask->cwd,path);
}
Task * getTaskForPID(int pid)
{
	Task *cur;
        cur = current;
        while(cur != NULL)
        {
	  if(cur->pid==pid)
                    return cur;
                cur=cur->next;
        }
return NULL;
}
void SYSS_waitpid(int pid)
{
	//	kprintf("\nWAITPID called");
	Task *cur;
        cur = current;
        while(cur != NULL)
        {
                if(strlen(cur->task_name)!=0 && cur->ppid == (uint64_t) pid )
                {
			if(cur->state==TASK_RUNNING)
			{
				Task * parent = getTaskForPID(pid);
				parent->state = TASK_WAITING;
				yield();
			}
		}
		cur=cur->next;
        }	
}
