#ifndef _STDARG_H
#define _STDARG_H

typedef char* va_list;
#define va_start(ap,last) ((ap) = (&(last) + 1))
#define va_end(ap)
int __va_arg(va_list *ap);
#define va_arg(ap, type) ((type)__va_arg(&(ap)))

#endif
