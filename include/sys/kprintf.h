#ifndef __KPRINTF_H
#define __KPRINTF_H

#include<stdarg.h>

void kprintf(const char *fmt, ...);
int kprintf1(const char *fmt, va_list args);
void print_tim_txt();
void print_tim(int sec);
#endif
