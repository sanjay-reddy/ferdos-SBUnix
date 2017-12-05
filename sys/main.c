#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/interrupts.h>
#include <sys/phymap.h>
#include <sys/task.h>
#include <sys/syscall.h>
#include <syscall.h>
#define INITIAL_STACK_SIZE 4096


typedef hba_mem_t HBA_MEM;
typedef hba_port_t HBA_PORT;
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
char stack[4096];
static size_t num_pages_base;   // Number of pages in base memory
static size_t num_pages_upper;
int max_mem;
void testContextSwitch()

{   
    kprintf("Switching to Task2\n");
//switch_to_ring3(userMain);    
initTasking();
    yield();
    kprintf("Returned to Task1\n");
}
void start(uint32_t *modulep, void *physbase, void *physfree)
{ 
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
    if(smap->base ==0) {
                num_pages_base = smap->length/PAGE_SIZE;
                //kprintf("Lower Memory Pages = %d\n",num_pages_base);
            }
            else {
                num_pages_upper =((smap->base+smap->length) - (uint64_t)physfree)/PAGE_SIZE;
                //kprintf("Higher Memory Pages = %d\n",num_pages_upper);
            }
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      max_mem = (smap->base + smap->length)/PAGE_SIZE;
     // kprintf("Max_mem %d", max_mem);      

//testContextSwitch();
}
 
}

mem_alloc(max_mem, (uint64_t)physfree, (uint64_t)physbase);
 
testContextSwitch();
//task_struct *process = create_userProcess("");
//switch_Ring3(process);
//kprintf("\nNumber of pages %d", max_mem);   
//load_cr3();

//tarfs_init();

kprintf("physfree %p\n", (uint64_t)physfree);
kprintf("physbase %p\n", (int64_t)physbase);
kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

//char *temptext = "test";

//sys_write(1,(uint64_t)temptext,5);
//syscall_3((uint64_t)1, (uint64_t)1,(uint64_t)temptext,(uint64_t)5);
//__asm__ volatile("sti");

}
void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
//  register char *temp1, *temp2;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  reload_idt();
//  reinit_PIC();
__asm__ volatile("sti");

//  __asm__ ("int $32");

 // tmp = checkAllBuses();

//  probe_port((HBA_MEM *)(0xFFFFFFFF00000000+(uintptr_t)bar5));

  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );

/*  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
*/
 while(1);
}
