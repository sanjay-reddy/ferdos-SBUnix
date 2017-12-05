#include <sys/interrupts.h>
/*
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );

}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void io_wait(void)
{
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}
*/

void
outb(uint16_t port, uint16_t val)
{
	__asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

char
inb(uint16_t port) {
	char ret;

	__asm__ volatile ("inb %%dx,%%al":"=a"(ret):"d"(port));

	return ret;
}
