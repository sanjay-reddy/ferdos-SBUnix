#include<syscall.h>
#include<sys/syscall.h>
#include<sys/defs.h>
#include<stdarg.h>
#include<sys/kprintf.h>
#include<string.h>
#include<unistd.h>

void printf_int(long tarf)
{

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
        p = var[l];
        while(p!='\0')
        {
                l++;
                p = var[l];
        }
	write(1,(char *)var,(uint64_t)(strlen(var)));


}

void printf_str(char *  string)
{

	write(1,(char *)string,(uint64_t)(strlen(string)));

}

void printf_char(long decimal)
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

        l=0;
        p = var[l];
        while(p!='\0')
        {
                l++;
                p = var[l];
        }
	
	write(1,(char *)var,(uint64_t)(strlen(var)));

}

void printf_def(char c)
{

	char str[2];
	str[0] = c;
	str[1] = '\0';
	write(1,(char *)str,(uint64_t)(strlen(str)));

}

void printf(char *fmt,...)
{
        int i = 0;
        va_list ap;
        va_start(ap,fmt);
        char c;
        int quit_flg = 0;
        long p_arg;
        int char_arg;
        do
         {
                c = fmt[i];
                switch(c)
                {
	                case '%':
                        i++;
                        c = fmt[i];
                        if (c=='p')
                                {
                                p_arg = va_arg(ap,unsigned long);
                                printf_char(p_arg);
                                //printf_char(p_arg);
                                }
                        if (c=='d')
                                {
                                p_arg = va_arg(ap,long);
                                printf_int(p_arg);
                                }
                        if (c=='c')
                                {
                                char_arg = va_arg(ap,int);
                                printf_def(char_arg);
                                }
                        if (c == 's')
                                {
                                printf_str(va_arg(ap,char *));
                                }
                        if(c == 'x')
                                {
                                printf_char(va_arg(ap,unsigned long));
                                }
                                break;
		
                case '\0':
                        quit_flg = -1;
                        break;

                default:
                        printf_def(c);
                }
                i++;

        }while(quit_flg != -1 );
va_end(ap);

}
