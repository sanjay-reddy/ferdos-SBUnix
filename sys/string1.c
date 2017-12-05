#include <sys/defs.h>
char * strstr(char* str, char* substr)
{       
        while(*str)
        {       
                char* first = str;
                char* pattern = substr;
                while(*str && *pattern && *str==*pattern)
                {
                str++;
                pattern++;
                }
                if(!*pattern)
                {       
                        return first;
                }
                str = first+1;
        }
        return NULL;
}

int strcmp(char *str1,char* str2)
{
        while(*str1 && * str2 && *str1==*str2)
        {
                str1++;
                str2++;
        }
        return (unsigned char)*str1 - (unsigned char)*str2;
}

char* strcat(char* str1, char* str2)
{
        char* result=str1;
        while(*str1)
        {
                str1++;
        }
        while(*str2)
        {
                *str1++=*str2++;
        }
        return result;
}

size_t strlen (char *str) {
    size_t len = 0;
    while (*str != '\0') {
        str++;
        len++;
    }
    return len;
}
void memset(void *ptr, int value, uint64_t num)
{
        int i = 0;
        for(; i < num; i++)
                ((char *)ptr)[i] = value;
}
void *memcpy(void *dest, const void *src, uint64_t n)
{
    unsigned char *pd = (unsigned char *)dest;
    const unsigned char *ps = (unsigned char *)src;
    if ( ps < pd )
        for (pd += n, ps += n; n--;)
            *--pd = *--ps;
    else
        while(n--)
            *pd++ = *ps++;
    return dest;
}
