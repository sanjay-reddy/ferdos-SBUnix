#ifndef _STRING_H
#define _STRING_H
char * strstr(char* str, char* substr);
int strcmp(char *str1,char* str2);
char* strcat(char* str1, char* str2);
size_t strlen (char *str);
void memset(void *ptr, int value, uint64_t num);
void *memcpy(void *dest, const void *src, uint64_t n);
#endif
