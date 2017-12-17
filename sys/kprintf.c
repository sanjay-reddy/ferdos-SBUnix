#include<stdarg.h>
#include<sys/kprintf.h>
#include<sys/defs.h>

/*
char c;
int colour = 2;
static volatile char * video = (volatile char * )0xB8000;
long int  startpos =  0xB8000;
static long int line_count = 0x1;
int quit_flg = 0;
*/

static volatile char * video = (volatile char * )0xffffffff800b8000UL;
static long int line_count = 0x1;
static int line_width = 1;
long int  startpos =  0xffffffff800b8000UL;
int colour = 2;
//static volatile char * start_scroll =(volatile char *)0xffffffff800b8000UL;

//long int scroll_check= 0xffffffff800B8DC0UL;
//static char * end_scroll = (char *)0xffffffff800B8DC0UL;

int kstrlen(const char *str)
{
        int len = 0;

        while(*str++ )
                len++;

        return len;
}


char* kstrcpy(char *dest,const char *src)
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

int kstr_to_int(char *str)
{
         int pos = 0;
        char buffer[10] = {'\0'};
        kstrcpy(buffer,str); 
        while ((buffer[pos] != '\0') && (buffer[pos] != '\n' ) ){
        pos++;
        }
        pos--;
        int val = 0;
        int io =1; 
        while (pos >= 0) {
                buffer[pos] = buffer[pos] - 48;
                val = val + buffer[pos]*io;
                pos--;
                io= io*10;
        }

        return val;

}

int kstrcmp(const char *str1,const char *str2)
{
        while(*str1 && *str2 && *str1 == *str2) {
                str1++;
                str2++;
        }

        return *str1 - *str2;
}

void print_char(unsigned long decimal)
{	
	
	char p;
	int l,k=0,i=1;
	int j=0;
	unsigned long quotient = decimal;
	int temp;
	char hexadec[1024];
	char var[1024];
	do{
		temp = quotient % 16;
		//To convert integer into character
		if( temp < 10)
		           temp =temp + 48; else
		         temp = temp + 55;
		hexadec[i++]= temp;
		quotient = quotient / 16;
	}
	while(quotient!=0); 
	for (j = i-1; j > 0; j--)
	{
           var[k]= hexadec[j];
	   k++;	
	}
	var[k]='\0';
	line_width = line_width + kstrlen(var);
	l=0;
	p = var[l];
	while(p!='\0')
	{
		*video++ = p;
		*video++ = colour;
		l++;
		p = var[l];
	}
}

void print_tim_txt()
{
	volatile char * tim_txt_adr = (char *)0xffffffff800B8EDCUL;
	char * text = "Timer";
	char t;
        int m=0;
        t = text[m];
        while(t!='\0')
        {
                *tim_txt_adr++ = t;
                *tim_txt_adr++ = colour;
                m++;
                t = text[m];
        }

}

void print_tim(int sec)
{
	volatile char * tim_adr = (char *)0xffffffff800B8EE8;
	char p;
        int l,k=0,i=1;
        int j=0;
        int quotient = sec;
        int temp;
        unsigned char hexadec[1024];
        char var[1024];
        while(quotient!=0) {
                temp = quotient%10;
                hexadec[i++]= temp + 48;
                quotient = quotient / 10;
        }
        hexadec[i]='\0';
        for (j = i-1; j > 0; j--)
        {
           var[k]= hexadec[j];
           k++;
        }
        var[k]='\0';
        l=0;
        p = var[l];
        while(p!='\0')
        {
                *tim_adr++ = p;
                *tim_adr++ = colour;
                l++;
                p = var[l];
        }
}

void print_str(unsigned long tarf)
{/*
	unsigned char * tarfs = (unsigned char*)&tarf;
	char p;
	int l=0;
        p = tarfs[l];
        while(p!='\0')
        {
                *video++ = p;
                *video++ = colour;
                l++;
                p = tarfs[l];
        }
*/

char p;
        int l,k=0,i=1;
        int j=0; 
        unsigned long quotient = tarf;
        int temp;
        unsigned char hexadec[1024];
        char var[1024];
	
	do{
                temp = quotient%10;
                hexadec[i++]= temp + 48;
                quotient = quotient / 10;
        }
        while(quotient!=0); 
        hexadec[i]='\0';
        for (j = i-1; j > 0; j--)
        {
           var[k]= hexadec[j];
           k++;
        }
        var[k]='\0';
        l=0;
	line_width += kstrlen(var);
        p = var[l];
        while(p!='\0')
        {
                *video++ = p;
                *video++ = colour;
                l++;
                p = var[l];
        }


}

void prints(char *str)
{
	int m = 0;
	char f;
	f = str[m];
		while(f!= '\0')
		{	
              	  *video++ = f;
              	  *video++ = colour;
                m++;
                f = str[m]; 
		}
	line_width += kstrlen(str);
}

void printbackspace()
{

	char b = (char)32;
	*video--;
	*video--;
	*video++ = b;
	*video++ = colour;
	*video--;
	*video--;

}


int kprintf1(const char *fmt, va_list args)
{
        int i = 0;
        char c;
        int quit_flg = 0;
        long p_arg;
        int c_arg;
        do
         {	
                c = fmt[i];
                switch(c)
                {
                case '\n':
                        video =(volatile char *)( ( line_count * 0xA0 ) + startpos);
                        line_count++;
                        break;
		case '\b':
			printbackspace();
			break;

                case '%':
                        i++;
                        c = fmt[i];
                        if (c=='p')
                                {
                                p_arg = va_arg(args,unsigned long);
                //              print_str(p_arg);
                                print_char(p_arg);
                                }
                        if (c=='d')
                                {
                                p_arg = va_arg(args,long);
                                print_str(p_arg);
                                }
                        if (c=='c')
                                {
                                c_arg = va_arg(args,int);
                                *video++ = c_arg;
                                *video++ = colour;
                                }
                        if (c == 's')
                                {
                                prints(va_arg(args,char *));
                                }
                        if(c == 'x')
                                {
                                print_char(va_arg(args,unsigned long));
                                }
                                break;
                case '\0':
                        quit_flg = -1;
                        break;

                default:
                        *video++ = c;
                        *video++ = colour;
                        break;
                }
                i++;

        }while(quit_flg != -1 );
	return i;
}


void screenclear()
{
	video =(volatile char *)(startpos);
	int m = 0;
	while(m<1840)
	{
		
		/**start_scroll++ = (char)32;
		*start_scroll++ = colour;*/
		kprintf(" ");
		line_width--;
		m++;
	}
	//start_scroll = (volatile char *)startpos;
	video = (volatile char *)startpos;
}

void kprintf(const char *fmt, ...)
{

        int i = 0; 
	va_list ap;
	va_start(ap,fmt);
	char c;
	int quit_flg = 0;
	long p_arg;
	int c_arg;
	do   
	 {	
		if(line_width>80)
		{
			line_count++;
		}
		c = fmt[i];
		switch(c)
		{
		case '\n':
			video =(volatile char *)( ( line_count * 0xA0 ) + startpos);
               		line_count++;
			line_width = 1;
			if(line_count >23)
			{
				screenclear();
				line_count = 1;
			}
			break;

                case '\b':
                        printbackspace();
			line_width--;
                        break;
		
		case '%':
			i++;
			c = fmt[i];
			if (c=='p')
				{
				p_arg = va_arg(ap,unsigned long);	
		//		print_str(p_arg);
				print_char(p_arg);			
				}
			if (c=='d')
				{
				p_arg = va_arg(ap,long);
				print_str(p_arg);				
				}
			if (c=='c')
				{
				c_arg = va_arg(ap,int);	
				*video++ = c_arg;
		                *video++ = colour;
				line_width++;
				}
			if (c == 's')
				{
				prints(va_arg(ap,char *));
				}
			if(c == 'x')
				{
				print_char(va_arg(ap,unsigned long));
				}
				break;	
		case '\0':
			quit_flg = -1;
			break;
		
		default:	
			*video++ = c;
		    	*video++ = colour;
			line_width++;
			break;
		}
		i++;	

	}while(quit_flg != -1 );


/*	
	if( c == '\n')
	{ 
	
	
		video =(volatile char *)( ( line_count * 0xA0 ) + startpos);
		line_count++;
	}
	
	i++;
        *video++ = c;
        *video++ = colour;
*/	
//	}while(c!= '\0');
	va_end(ap);

}

