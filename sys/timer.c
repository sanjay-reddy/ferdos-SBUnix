#include <sys/defs.h>
#include<sys/kprintf.h>
#include <sys/interrupts.h>


static long sec = 0;
static int timer_ticks = 0;
void tick_timer(struct isr_regs *r)
{
	outb(0x20,0x20);
	print_tim_txt();	
	timer_ticks++;
	if((timer_ticks % 18) == 0) {
		sec++;
		print_tim(sec);	
	// to implement the timer kprintf codes
	}

}
