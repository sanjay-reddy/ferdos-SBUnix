#ifndef _PHYMAP_H
#define _PHYMAP_H

#include <sys/defs.h>
#include <sys/task.h>
#include <sys/elf64.h>
/* Page table/directory entry flags */
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define FRAME		0xFFFFFFFFFFFFF000 // the permission bits.
#define VIDEO_MEMORY    0xb8000UL
#define PAGE_SIZE 0x1000
uint64_t cr3;

struct Page{
	struct Page * next;
	int refer_cnt;
	uint64_t addr;
};


struct PML4{
	uint64_t pt_entries[512];
};
struct PDPT{
        uint64_t pt_entries[512];
};
struct PDT{
        uint64_t pt_entries[512];
};
struct PT{
        uint64_t pt_entries[512];
};

void * kmalloc();

void pagetable_init();

void page_init();

void mem_alloc(int no_of_pages, uint64_t phys_free, uint64_t end_address);

struct Page * page_alloc();

void page_dealloc(struct Page *ptr);

struct PDPT * pdpt_alloc(struct PML4 * pml4,uint64_t pml4_index);

struct PDT * pdt_alloc(struct PDPT * pdpt,uint64_t pdpt_index);

struct PT * pt_alloc(struct PDT * pdt,uint64_t pdt_index);

void pagetable_init();

void map_virt_address(uint64_t vaddr, uint64_t paddr);

void mapAllpages();

void map_process(uint64_t,uint64_t);

uint64_t get_pml4_entry(struct PML4 **pml4, uint64_t pml4_indx);

uint64_t get_pdpt_entry(struct PDPT** pdpt, uint64_t pdpt_indx);

uint64_t get_pdt_entry(struct PDT **pdt, uint64_t pdt_indx);

uint64_t get_pt_entry(struct PT **pt, uint64_t pt_indx);

void* get_phy_addr(uint64_t vaddr);

vma_struct * addVMA(Elf64_Ehdr *ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz);
#endif
