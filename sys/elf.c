#include <sys/defs.h>
#include <sys/elf64.h>
#include <sys/tarfs.h>
#include <sys/phymap.h>
#include <sys/task.h>
#include <string.h>
char* sastrcpy(char *dest,const char *src)
{       
        char *saved = dest;
        
        while(*src){
                *dest = *src;
                src++;
                dest++;
        }
        *dest = '\0';
        
        return saved;
}
void elf_load(struct Task *task, char *file_name,int argc, char **args){
	/*argc=0;
	char c_ptr[6][64];
	int i;	
	for (i = 0; i < 6; ++i)
       		c_ptr[i][0] = '\0';


	//push the filename on the userstack 
	sastrcpy(c_ptr[argc], file_name);
	uint64_t c_argc = 0;
	argc++;

	if(args != NULL) {
		while(c_argc<6){
              		sastrcpy(c_ptr[argc],"Mankatha");
               		argc++;
			c_argc++;
        	}
	}*/	
   uint64_t addr = getElfHeader(file_name);
   if(addr!=0){
          Elf64_Ehdr *elfHeader = (Elf64_Ehdr *)addr;
	  Elf64_Phdr *ph1=(Elf64_Phdr *)((uint64_t)elfHeader + elfHeader->e_phoff);
	  task->task_rip = elfHeader->e_entry;
          int ph_cnt = elfHeader->e_phnum;
	  vma_struct *vma = NULL;
       	  for(int i=0;i<ph_cnt;i++,ph1++){		
          	if(ph1->p_type==1){
                        vma = (vma_struct *)addVMA(elfHeader,ph1->p_vaddr,ph1->p_vaddr + ph1->p_memsz,ph1->p_flags,ph1->p_offset,NOTYPE,ph1->p_filesz);
         	        task->vma=vma;
			// allocating the memory for the 
			uint64_t size = (((vma->end/0x1000)*0x1000 + 0x1000)-((vma->start/0x1000)*0x1000));
         		uint64_t itr = size/0x1000;
         		uint64_t start = (ph1->p_vaddr/0x1000)*0x1000; //p_vaddr give the virtual address at which the first byte of segment in memory
         		while(itr)  // block by block mapping to vma
         		{
          			uint64_t page = (uint64_t)page_alloc()->addr;
          			map_process(start, page);
          			itr--;
          			start += 0x1000;
        		}
			if((ph1->p_flags == (PERM_R | PERM_X)) || (ph1->p_flags == (PERM_R | PERM_W)))
 	                {
                	         memcpy((void*)vma->start, (void*)((uint64_t)elfHeader + ph1->p_offset), ph1->p_filesz);
        	        }
                 }
         }
        //allocate the vma for stack and heap.
	get_phy_addr(HEAP_START);
	vma_struct * vma_heap = addVMA(NULL,HEAP_START,HEAP_START + PAGE_SIZE,PERM_R | PERM_W,(uint64_t)NULL,HEAP,(uint64_t)NULL);
	if(vma != NULL)
        	vma->next = vma_heap;
	//stack
	uint64_t *stack_addr = (uint64_t *)get_phy_addr(USER_STACK_TOP-0x1000);
	vma_struct * vma_stack = addVMA(NULL,(uint64_t)stack_addr + PAGE_SIZE,(uint64_t)stack_addr,PERM_R | PERM_W,(uint64_t)NULL,STACK,(uint64_t)NULL);
	vma_heap->next = vma_stack;
	//argc=1;
	//char* argg="Mankat";
	uint64_t *ptr = (uint64_t *)((uint64_t)stack_addr +0x1000 - 16 - 64*argc);
	if(argc>0)
	{
        *ptr=argc;
	memcpy((ptr+1), (void *)args, argc*64);
        }
	task->task_rsp = (uint64_t)(ptr);
	
	//task->task_rsp = (uint64_t)((uint64_t)stack_addr + 511);;
	task->kernel_rsp = (uint64_t)(&task->kstack[511]);
  }
}
