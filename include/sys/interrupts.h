#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <sys/defs.h>

#define PF_P	0x1
#define PF_W	0x2
#define PF_U	0x4

typedef struct isr_regs registers_t;

struct isr_regs	{
    	uint64_t r9;
	uint64_t r8;

	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rdi;

	uint64_t int_no;
	uint64_t err_code;

	uint64_t rip;
    	uint64_t cs;
    	uint64_t rflags;
    	uint64_t rsp;
    	uint64_t ss;
}__attribute__((packed));

/* Defines an IDT entry */
struct idt_entry
{	uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t type_attr; // type and attributes
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero;     // reserved
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    uint64_t base;
} __attribute__((packed));


extern void _x86_64_asm_ildt(struct idt_ptr* );

/* IO.C */
/*
static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline void io_wait(void);
*/
void outb(uint16_t port, uint16_t val);
char inb(uint16_t port);

/* ISR.C */
extern void idt_set_gate(unsigned long num, unsigned long base, unsigned short sel, unsigned char flags);
extern void reload_idt();
extern void init_isr();
extern void reinit_PIC();
extern void interrupt_handler(struct isr_regs *);

/* TIMER.C */
extern void install_timer();
extern void tick_timer(struct isr_regs*);
extern void keyboard_inter_key(struct isr_regs *r);	


#endif
