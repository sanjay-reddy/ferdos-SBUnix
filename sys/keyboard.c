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

static volatile int flag = 0;
static char tempbuff[1024];
static volatile int scanlen = 0;

char* strcpy(char *dest,const char *src)
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


uint64_t scanf(void * buff,int len)
{
        flag = 1;

	__asm__ __volatile__("sti;"); 

	//kprintf("\ngoing in while\n");
 	while(flag ==1);
	
	//kprintf("Coming out of While\n");
	
	if(len<scanlen){
		scanlen = len;
	}
//	kprintf("%s",tempbuff);
	strcpy(buff,tempbuff);
	memcpy((void*)buff,(void*)tempbuff,scanlen);
	while(scanlen >=0){
	tempbuff[scanlen]= '\0';
	scanlen--;
	}
	len = scanlen;
	scanlen=0;
	return len;

}

int shift_key = 0;

void keyboard_inter_key(struct isr_regs *reg)
{
    
	unsigned char check_code;
  
  	check_code = inb(0x60);	
	outb(0x20, 0x20);
	if (check_code == 28){
        flag=0;
	}
	if(check_code == 14)
	{
		kprintf("\b");
		tempbuff[scanlen--]='\0';
		return;
        }
	if (check_code & 0x80){
		if(check_code == 0x2A)
  		{
                     shift_key = 0;
		}
	}
	else
  	{
		if(check_code == 0x2A)
		{
                      shift_key = 1;
		}
		else{
			if(shift_key == 1)
			{
				if(kbcaps[check_code]=='\n')
					kprintf("\n");
				else{
					kprintf("%c",kbcaps[check_code]);
					if(flag==1){
						if(check_code == 14){
							scanlen--;
						}
						else
							tempbuff[scanlen++]=kbcaps[check_code];
					}
				}
				shift_key = 0;
			}
	
			else
			{
				if(kbsmall[check_code]=='\n')
					kprintf("\n");
				else
				{
					kprintf("%c",kbsmall[check_code]);
					if(flag==1)
					{
						if(check_code == 14){
							scanlen--;
						}
						else
							tempbuff[scanlen++]=kbsmall[check_code];
					}
				}
			}	
		}
	}
}

void keyboard_inter_key1(struct isr_regs *r)
{
	char val;
	uint8_t scancode;
	scancode = inb(0x60);
        outb(0x20,0x20);
	if(scancode<59){
	
	if(scancode == 14 )
	{
		kprintf("\b");
		tempbuff[scanlen--]='\0';
		return;
	}	
	
	if(scancode ==28)
		{	
			
			if(flag==1)
				flag = 0;
			kprintf("\n");
		}

	if(lastkey == 42)
	{
		val = kbcaps[scancode];
		if(scancode!=42)
		kprintf("%c",val);	
		if (flag == 1)
		{
			//strcat(tempbuff,(char)val);
			tempbuff[scanlen++] = val;
		}
	}
	else
	{
		if(lastkey == 29 )
		{
			kprintf("^");
			val = kbcaps[scancode];
			kprintf("%c",val);

		if (flag == 1)
		{
			tempbuff[scanlen++] = val;
		}

		}

		else
		{
		val = kbsmall[scancode];
		kprintf("%c",val);
		if (flag == 1)
		{
			tempbuff[scanlen++] = val;
		}

		}
	
	}
        lastkey = scancode;
	}
}


