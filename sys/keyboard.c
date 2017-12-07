#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/interrupts.h>
#include <string.h>
static uint8_t lastkey;

unsigned char kbsmall[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r',   /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`',   0,      /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', ',', '.', '/',   0,              /* Right shift */
    '*',
    0,    /* Alt */
    ' ',  /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num Lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
    '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
    '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
};

/* Keyboard mapping on SHIFT press */
unsigned char kbcaps[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* Curly Brackets */
    '(', ')', '_', '+', '\b', /* Backspace */
    '\t',           /* Tab */
    'Q', 'W', 'E', 'R',   /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
    '\"', '~',   0,      /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
    'M', '<', '>', '?',   0,              /* Right shift */
    '*',
    0,    /* Alt */
    ' ',  /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num Lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
    '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
    '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
};


/*
uint64_t scanf(char * buff)
{
        uint8_t val;
        uint8_t scancode;
        scancode = inb(0x60);
        outb(0x20,0x20);
        if(scancode<59){

        if(scancode ==28)
                {
                        kprintf("%s\n",buff);
                }

        if(lastkey == 42)
        {
                val = kbcaps[scancode];
                if(scancode!=42)
                kprintf("%c",val);
        }
        else
        {
                if(lastkey == 29 )
                {
                        kprintf("^");
                        val = kbcaps[scancode];
                        kprintf("%c",val);
                }
                else
                {
                val = kbsmall[scancode];
                kprintf("%c",val);
                }

        }
        lastkey = scancode;
        }
}
*/

void keyboard_inter_key(struct isr_regs *r)
{
	uint8_t val;
	uint8_t scancode;
	scancode = inb(0x60);
        outb(0x20,0x20);
	if(scancode<59){
	
	if(scancode == 14 )
	{
		kprintf("\b");
	}	
	
/*	if(scancode ==28)
		{
			kprintf("%s\n",buff);
		}
*/
	if(lastkey == 42)
	{
		val = kbcaps[scancode];
		if(scancode!=42)
		kprintf("%c",val);	
	}
	else
	{
		if(lastkey == 29 )
		{
			kprintf("^");
			val = kbcaps[scancode];
			kprintf("%c",val);
		}
		else
		{
		val = kbsmall[scancode];
		kprintf("%c",val);
		}
	
	}
        lastkey = scancode;
	}
}


