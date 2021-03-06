#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/phymap.h>
#include <sys/task.h>
#include <string.h>
struct Page *pages;
static struct Page * free_list;
static int noofpages;
static uint64_t  physfree;
struct PML4 *pml4;
extern char kernmem,physbase;
static uint64_t maxPhysMem;
uint64_t start_vAddr = IDEN_V;
void load_CR3(){
	uint64_t base_pgdir_addr = (uint64_t)cr3;
	__asm volatile("movq %0,%%cr3"::"b"(base_pgdir_addr));
}
/* get/set helper CR3 functions */
uint64_t get_CR3()
{
	uint64_t saved_cr3;
	__asm volatile("mov %%cr3, %0" : "=r" (saved_cr3));

	return saved_cr3;
}

void set_CR3(struct PML4 *new_cr3)
{
	uint64_t base_pgdir_addr = (uint64_t)new_cr3;
	__asm volatile("mov %0, %%cr3":: "b"(base_pgdir_addr));
}
struct vm_area_struct* allocate_vma(struct vm_area_struct **vma_head){
    struct vm_area_struct * tail;
    if(*vma_head == NULL){
            *vma_head=(struct vm_area_struct *)(kernmem+page_alloc()->addr);
            return *vma_head;
        }
    else{
        tail=*vma_head;
        while(tail->next!=NULL)
            tail=tail->next;
        tail->next=(struct vm_area_struct *)(sizeof(struct vm_area_struct)+(char *)tail);
        return tail->next;
    }
	return NULL;
}
void mem_alloc(int no_of_pages,uint64_t  phys_free,uint64_t  end_address)
{
	noofpages=no_of_pages;
	physfree = phys_free;
	maxPhysMem = end_address;
	pages=(struct Page *)physfree;
	page_init();
	pagetable_init();
	mapAllpages();
	map_virt_address((uint64_t)0xffffffff800b8000UL, VIDEO_MEMORY);	
	load_CR3();
//	uint64_t addressss = (uint64_t)page_alloc()->addr;
	//kprintf("Summa-sdgsdg %x",addressss);
}
void page_init()
{
	struct Page* prevPtr = NULL;
	size_t i;
	size_t memory = (physfree+noofpages*sizeof(struct Page *))/4096;
	for(i=0;i<noofpages;i++){
		pages[i].addr = i*4096;
		if(i < memory)
		{
			pages[i].refer_cnt = 1;
			pages[i].next = NULL;
		}
		else {
			pages[i].refer_cnt = 0;
			if(prevPtr!=NULL)
			{
				prevPtr->next = &pages[i];
			}
			else{
			free_list = &pages[i];
			}
			prevPtr = &pages[i];
		}
	}
}
struct Page * page_alloc()
{
	struct Page * newPage=NULL;
	if(free_list == NULL)
	{
		return NULL;
	}
	newPage = free_list;
	free_list = free_list->next;
	return newPage;
}
void page_dealloc(struct Page *ptr)
{
	ptr->next = free_list;
	free_list=ptr;

}
uint64_t getPML4Index(uint64_t addr)
{
	return ((addr) >> 39) & 0x1FF;
}
uint64_t getPDPTIndex(uint64_t addr)
{
	return ((addr) >> 30) & 0x1FF;
}
uint64_t getPDTIndex(uint64_t addr)
{
	return ((addr) >> 21) & 0x1FF;
}
uint64_t getPTIndex(uint64_t addr)
{
	return ((addr) >> 12) & 0x1FF;
}
struct PDPT * pdpt_alloc(struct PML4 * pml4,uint64_t pml4_index)
{
	struct PDPT *pdpt = (struct PDPT *)page_alloc()->addr;
	uint64_t pdpt_entry = (uint64_t)pdpt;
	pdpt_entry |= (PTE_P|PTE_W|PTE_U);
	pml4->pt_entries[pml4_index] = pdpt_entry;
	return (void *)pdpt;
}
struct PDT * pdt_alloc(struct PDPT * pdpt,uint64_t pdpt_index)
{
	struct PDT *pdt = (struct PDT *)page_alloc()->addr;
	uint64_t pdt_entry = (uint64_t)pdt;
	pdt_entry |= (PTE_P|PTE_W|PTE_U);
	pdpt->pt_entries[pdpt_index] = pdt_entry;
	return (void *)pdt;
}
struct PT * pt_alloc(struct PDT * pdt,uint64_t pdt_index)
{
	struct PT *pt = (struct PT *)page_alloc()->addr;
	uint64_t pt_entry = (uint64_t)pt;
	pt_entry |= (PTE_P|PTE_W|PTE_U);
	pdt->pt_entries[pdt_index] = pt_entry;
	return (void *)pt;
}
void pagetable_init()
{
	pml4  = (struct PML4 *)page_alloc()->addr;
	cr3=(uint64_t)pml4;
	struct PDPT * pdpt;
	struct PDT * pdt;
	struct PT * pt;

	uint64_t vaddr = (uint64_t)&kernmem;
	uint64_t pbase = (uint64_t)&physbase;
	uint64_t pml4_index = getPML4Index(vaddr);
	uint64_t pdpt_index = getPDPTIndex(vaddr);
	uint64_t pdt_index = getPDTIndex(vaddr);
	if(!pml4) return;
	pdpt = pdpt_alloc(pml4,pml4_index);
	if(!pdpt) return;
	pdt = pdt_alloc(pdpt,pdpt_index);
	if(!pdt) return;
	pt = pt_alloc(pdt,pdt_index);
	if(!pt) return;
	while(pbase<(physfree))
	{
		uint64_t pt_index = getPTIndex(vaddr);
//		kprintf("%x,",pt_index);
		uint64_t base = pbase;
		base |= (PTE_P|PTE_W|PTE_U);
		pt->pt_entries[pt_index] = base; 
		pbase+=0x1000;
		vaddr+=0x1000;
	}
}
void mapAllpages()
{
	uint64_t viraddr = physfree;
	while(viraddr<=0x500000)
	{
		map_virt_address(viraddr,viraddr);
		viraddr+=0x1000;
	}
//	map_virt_address((uint64_t)0xffffffff800b8000UL, VIDEO_MEMORY);

}
uint64_t get_address(uint64_t* entry)
{
	return (*entry & FRAME);
}
void map_virt_address(uint64_t vaddr,uint64_t paddr){
        struct PDPT *pdpt;
        struct PDT *pdt;
        struct PT *pt;

        uint64_t pml4Index = getPML4Index((uint64_t)vaddr);
        uint64_t pdptIndex = getPDPTIndex((uint64_t)vaddr);
        uint64_t pdtIndex = getPDTIndex((uint64_t)vaddr);
        uint64_t ptIndex = getPTIndex((uint64_t)vaddr);

        uint64_t pml4_entry =  pml4->pt_entries[pml4Index];
        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);
        else
                pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4Index);

        uint64_t pdpt_entry = pdpt->pt_entries[pdptIndex];
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);
        else
                pdt = (struct PDT*)pdt_alloc(pdpt, pdptIndex);

        uint64_t pdt_entry = pdt->pt_entries[pdtIndex];
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
                pt = (struct PT*)pt_alloc(pdt, pdtIndex);

        uint64_t entry = paddr;
        entry |= (PTE_P|PTE_W|PTE_U);
        pt->pt_entries[ptIndex] = entry;

}
uint64_t kmalloc(uint64_t size)
{
        if(size>0){
                int cnt=0;
                int i=0;
                cnt = size/4096;
                if(size%4096>0)
                        cnt++;
                uint64_t ret_addr;
                ret_addr=(uint64_t)(page_alloc()->addr);
                for(i=1;i<cnt;i++){
                        page_alloc();
                }
                return ret_addr;
        }
        return 0;
}
/* returns the pml4 entry */
uint64_t get_pml4_entry(struct PML4 **pml4, uint64_t pml4_indx)
{
	//*pml4 = (struct PML4 *)((uint64_t)*pml4 | IDEN_V);
	uint64_t pml4_entry = (*pml4)->pt_entries[pml4_indx];
	
	return pml4_entry;
}

/* returns the pdpt entry */
uint64_t get_pdpt_entry(struct PDPT** pdpt, uint64_t pdpt_indx)
{
	//*pdpt = (struct PDPT *)((uint64_t)*pdpt | IDEN_V);
	uint64_t pdpt_entry = (*pdpt)->pt_entries[pdpt_indx];

	return pdpt_entry;	
}

/* returns the pdt entry */
uint64_t get_pdt_entry(struct PDT **pdt, uint64_t pdt_indx)
{
	//*pdt = (struct PDT *) ((uint64_t) *pdt | IDEN_V);
	uint64_t pdt_entry = (*pdt)->pt_entries[pdt_indx];

	return pdt_entry;
}

/* returns the pt entry */
uint64_t get_pt_entry(struct PT **pt, uint64_t pt_indx)
{
	//*pt = (struct PT *)((uint64_t) *pt | IDEN_V);
	uint64_t pt_entry = (*pt)->pt_entries[pt_indx];

	return pt_entry;
}
void map_process(uint64_t vaddr,uint64_t paddr)
{
        struct PDPT     *pdpt;
        struct PDT      *pdt;
        struct PT       *pt;

	uint64_t pml4_indx = getPML4Index((uint64_t)vaddr);
        uint64_t pdpt_indx = getPDPTIndex((uint64_t)vaddr);
        uint64_t pdt_indx = getPDTIndex((uint64_t)vaddr);
        uint64_t pt_indx = getPTIndex((uint64_t)vaddr);

	struct PML4 *pml4 = (struct PML4*) get_CR3();

	//pml4 = (struct PML4*) (IDEN_V | (uint64_t) pml4); 
        uint64_t pml4_entry = pml4->pt_entries[pml4_indx];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);
        else
		pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4_indx); 


	uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);
        else
		pdt = (struct PDT*)pdt_alloc(pdpt, pdpt_indx);


	uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
		pt = (struct PT*)pt_alloc(pdt, pdt_indx);


	//pt = (struct PT*)((uint64_t) pt | IDEN_V);
        uint64_t entry = paddr;
        entry |= (PTE_P|PTE_W|PTE_U);

        pt->pt_entries[pt_indx] = entry;

}
void* get_phy_addr(uint64_t vaddr)
{
	uint64_t page = (uint64_t)page_alloc()->addr;
	map_process(vaddr, page);	
	return (void *)vaddr;
}
vma_struct * addVMA(Elf64_Ehdr *ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz) // should be called in loop for each phdr
{
vma_struct *vma = (vma_struct *)kmalloc((uint64_t)4096);
vma->start = start;
vma->end =  end;
vma->flags = flags;
vma->next = NULL;
vma->type = type;

if(flags == (PERM_R | PERM_X)) // text segment is only readable/executable
{

       vma->type = TEXT;

}

else
{
    vma->type = DATA;
}

return vma;
}
uint64_t set_user_AddrSpace()
{       
        struct PML4 *newPML4 = (struct PML4 *)page_alloc()->addr;
        struct PML4 *curPML4 = (struct PML4 *)get_CR3();
        
        //curPML4 = (struct PML4 *)((uint64_t)curPML4 | IDEN_V);
        
        /* map the kernel page table for the process */
	for(int i=0;i<=511;i++)  
	newPML4->pt_entries[i] = curPML4->pt_entries[i];
        
        return (uint64_t)newPML4;
}
void copy_page_tables_forchild(uint64_t child_PML4)
{       
        struct PML4 *c_pml4 = (struct PML4 *)child_PML4;
        struct PML4 *p_pml4 = (struct PML4 *)get_CR3();
        
        int pml4_indx = 0;
        for(; pml4_indx < 510; pml4_indx++) {
                
                uint64_t pml4_entry = get_pml4_entry(&p_pml4, pml4_indx);
                
                if(pml4_entry & PTE_P) {
                        
                        struct PDPT *c_pdpt = (struct PDPT *)pdpt_alloc(c_pml4, pml4_indx);
                        
                        struct PDPT *p_pdpt = (struct PDPT *) get_address(&pml4_entry);
                        int pdpt_indx = 0;
                        for(; pdpt_indx < 512; pdpt_indx++) {
                                
                                uint64_t pdpt_entry = get_pdpt_entry(&p_pdpt, pdpt_indx);
                                if(pdpt_entry & PTE_P) {
                                        
                                        //struct PDPT *tmp_pdpt = c_pdpt; 
                                        struct PDT *c_pdt = (struct PDT *)pdt_alloc(c_pdpt, pdpt_indx);
                                        
                                        struct PDT *p_pdt = (struct PDT *) get_address(&pdpt_entry);
                                        int pdt_indx = 0;
                                        for(; pdt_indx < 512; pdt_indx++) {
                                                
                                                uint64_t pdt_entry = get_pdt_entry(&p_pdt, pdt_indx);
                                                if(pdt_entry & PTE_P) {
                                                        
                                                        //struct PDT *tmp_pdt = c_pdt;
                                                        struct PT *c_pt = (struct PT *)pt_alloc(c_pdt, pdt_indx);
                                                        
                                                        struct PT *p_pt = (struct PT *)get_address(&pdt_entry);
                                                        int pt_indx = 0;
                                                        for(; pt_indx < 512; pt_indx++) {
                                                                
                                                               uint64_t pt_entry = get_pt_entry(&p_pt, pt_indx);
                                                                if(pt_entry & PTE_P) {
                                                                        uint64_t page = (uint64_t)get_address(&pt_entry);
                                                                        pt_entry = page | (PTE_P | PTE_U | PTE_COW);
                                                                        
  		                                                        c_pt->pt_entries[pt_indx] = pt_entry;
                						//	char temp[4096];
								//	memcpy((void*)&temp[0],(void*)p_pt->pt_entries[pt_index],4096);
										                                                        
                                                                        pt_entry = page | (PTE_P | PTE_U |PTE_COW);
                                                                        p_pt->pt_entries[pt_indx] = pt_entry;
									//switchtochildcr3
									//memcpy((void*)c_pt->pt_entries[pt_indx],(void*)p_pt->pt_entries[pt_indx],4096);
									//switchbacktoparentCR3
									
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
        c_pml4->pt_entries[511] = p_pml4->pt_entries[511];
        c_pml4->pt_entries[510] = p_pml4->pt_entries[510];
}

uint64_t phyAddr(uint64_t vaddr)
{
	uint64_t	paddr = 0;
	struct PDPT     *pdpt = NULL;
        struct PDT      *pdt = NULL;
        struct PT       *pt = NULL;

        uint64_t pml4_indx = getPML4Index((uint64_t)vaddr);
        uint64_t pdpt_indx = getPDPTIndex((uint64_t)vaddr);
        uint64_t pdt_indx = getPDTIndex((uint64_t)vaddr);
        uint64_t pt_indx = getPTIndex((uint64_t)vaddr);

        struct PML4 *pml4 = (struct PML4*) get_CR3();
        uint64_t pml4_entry = pml4->pt_entries[pml4_indx];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);

        uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);


        uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);

	/* page table entry is not null, extract the physical address */ 
	if(pt != NULL)
		paddr = get_pt_entry(&pt, pt_indx);

	return paddr;	
}
