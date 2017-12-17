#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#include <sys/defs.h>
#include <sys/interrupts.h>
#include<string.h>
#include<sys/kprintf.h>
#include<sys/syscall.h>
#include<syscall.h>
#include <sys/phymap.h>
extern void isr_timer();
extern void isr_keyboard();
extern void isr_syscall();
extern void page_fault_handler();


struct idt_entry idt[256];	// initialising IDT 
struct idt_ptr idtp;


void idt_set_gate(unsigned long num, unsigned long base, unsigned short sel,
                  unsigned char flags)
{

	idt[num].offset_1 = (base) & 0xFFFF;
        idt[num].selector = sel;
        idt[num].ist = 0;
        idt[num].type_attr = flags;
        idt[num].offset_2 = (base >> 16) & 0xFFFF;
        idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
        idt[num].zero = 0;


}

void reload_idt()
{
	idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
	idtp.base = (uint64_t)&idt;

	memset(&idt, 0, sizeof(struct idt_entry) * 256);

 	init_isr();

        __asm__ __volatile__ (  "lidt (%0)\n"
                              :
                              :"r"(&idtp)
                              :
                               );
	
}
/*
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}
*/
void reinit_PIC()
{
	
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	outb(0x21, 0x20); // Master PIC vector offset
	outb(0xA1, 0x28); // Slave PIC vector offset

	outb(0x21, 0x04); // informs Master PIC about Slave PIC cascaded at ICW2 
	outb(0xA1, 0x02); // cascade identity of Slave PIC

	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	outb(0x21, 0x0);
	outb(0xA1, 0x0);

}



void init_isr()
{
	
	idt_set_gate(32, ((uint64_t)isr_timer),0x08, 0x8E);
        idt_set_gate(33, ((uint64_t)isr_keyboard),0x08, 0x8E);
	idt_set_gate(128, ((uint64_t)isr_syscall),0x08, 0xEE);
	idt_set_gate(14,(uint64_t)page_fault_handler,0x08,0x8E);
	//reinit_PIC();
}


/*void page_fault_handler()
{
	kprintf("page fault has occured\n");
}*/
/*
void execute_interrupt(struct isr_regs *reg)
{
	if(reg->int_no == 32)	
		tick_timer(reg);
	if(reg->int_no == 33 )
		keyboard_inter_key(reg);
	if(reg->int_no == 128 )
		syscall_handler(reg);
	if(reg->int_no == 13)
		// to handle gpf
	if(reg->int_no == 14)
		page_fault_handler();

}
*/

void interrupt_handler_syscall(struct isr_regs *reg)
{
	syscall_handler(reg);
//	outb(0x20, 0x20);

}


void interrupt_handler_key(struct isr_regs *reg)
{
        keyboard_inter_key(reg);
}

void interrupt_handler_timer(struct isr_regs *reg)
{
	tick_timer(reg);
}
void pg_fault_handler(struct isr_regs *reg)
{
	volatile uint64_t faultAddr;

	__asm volatile("mov %%cr2, %0" : "=r" (faultAddr));


//	uint64_t err = reg->err_code & 0xF;

	// COW page fault handling
//	if((err & PF_P) && (err & PF_W)) {

		uint64_t pte_entry = phyAddr(faultAddr);
		if(pte_entry & PTE_COW) {
			uint64_t temp_vaddr = (uint64_t)kmalloc(4096);
			uint64_t paddr = phyAddr(get_address(&temp_vaddr));
			//uint64_t v = (faultAddr&(0xffffffffffffffffUL<<12));
			faultAddr = ((faultAddr / 4096) * 4096);
			//memcpy((void *)temp_vaddr, (void *)(faultAddr&(0xffffffffffffffffUL<<12)), 4096);
			memcpy((void *)temp_vaddr, (void *)(faultAddr), 4096);
			//map_process(faultAddr&(0xffffffffffffffffUL<<12), paddr);
			map_process(faultAddr, paddr);

			pte_entry = (pte_entry & ~0xfff)| PTE_P | PTE_W | PTE_U; 
			return;
		}	
//	}
}

/*
void interrupt_syscall(struct isr_regs *reg)
{
	syscall_handler(reg);
}
*/

