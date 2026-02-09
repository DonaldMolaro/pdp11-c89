#include <stdarg.h>
#include <stddef.h>

/* Declarations provided by runtime assembly emitted by codegen. */
int putchar(int c);
int getchar(void);

/* Minimal FILE stub */
typedef struct FILE FILE;
struct FILE { int dummy; };
FILE *stdin = 0;
FILE *stdout = 0;
FILE *stderr = 0;

/* Simple bump allocator with size headers. */
#define HEAP_SIZE 30000
static unsigned char heap[HEAP_SIZE];
static unsigned int heap_pos;

static void *alloc_raw(size_t size) {
    unsigned int total;
    unsigned int *hdr;

    if (size == 0) size = 1;
    if (size & 1) size++;
    total = (unsigned int)size + (unsigned int)sizeof(unsigned int);
    if (heap_pos + total > HEAP_SIZE) {
        /* out of memory: spin */
        for (;;) {}
    }
    hdr = (unsigned int *)(heap + heap_pos);
    *hdr = (unsigned int)size;
    heap_pos += total;
    return (void *)(hdr + 1);
}

void *malloc(size_t size) {
    return alloc_raw(size);
}

void *calloc(size_t nmemb, size_t size) {
    size_t n = nmemb * size;
    unsigned char *p = (unsigned char *)alloc_raw(n);
    size_t i;
    for (i = 0; i < n; i++) p[i] = 0;
    return p;
}

void free(void *ptr) {
    (void)ptr;
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) return alloc_raw(size);
    {
        unsigned int *hdr = ((unsigned int *)ptr) - 1;
        unsigned int old = *hdr;
        if (size <= old) return ptr;
        {
            unsigned char *p = (unsigned char *)alloc_raw(size);
            unsigned int i;
            for (i = 0; i < old; i++) p[i] = ((unsigned char *)ptr)[i];
            return p;
        }
    }
}

void exit(int code) {
    (void)code;
    for (;;) {}
}

/* Ctype */
int isspace(int c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

/* String/memory */
size_t strlen(const char *s) {
    size_t n = 0;
    while (s && s[n]) n++;
    return n;
}

int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
    size_t i = 0;
    if (n == 0) return 0;
    while (i + 1 < n && a[i] && b[i] && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return c == 0 ? (char *)s : NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == 0) return (char *)s;
    return (char *)last;
}

char *strstr(const char *hay, const char *needle) {
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    while (*hay) {
        if (strncmp(hay, needle, nlen) == 0) return (char *)hay;
        hay++;
    }
    return NULL;
}

void *memcpy(void *dst, const void *src, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = ((const unsigned char *)src)[i];
    return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
    size_t i;
    if (dst < src) {
        for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = ((const unsigned char *)src)[i];
    } else if (dst > src) {
        for (i = n; i > 0; i--) ((unsigned char *)dst)[i - 1] = ((const unsigned char *)src)[i - 1];
    }
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = (unsigned char)c;
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) {
        int da = ((const unsigned char *)a)[i];
        int db = ((const unsigned char *)b)[i];
        if (da != db) return da - db;
    }
    return 0;
}

/* Formatting */
struct OutBuf {
    char *buf;
    int idx;
    int to_buf;
};

static void outch(struct OutBuf *o, int c) {
    if (o->to_buf) {
        o->buf[o->idx++] = (char)c;
    } else {
        putchar(c);
    }
}

static void outstr(struct OutBuf *o, const char *s, int maxlen) {
    int i = 0;
    if (!s) s = "(null)";
    while (s[i] && (maxlen < 0 || i < maxlen)) {
        outch(o, s[i]);
        i++;
    }
}

static void outpad(struct OutBuf *o, int count, int ch) {
    while (count-- > 0) outch(o, ch);
}

static void outnum(struct OutBuf *o, unsigned long v, int base, int upper, int width, int pad_zero, int neg) {
    char tmp[32];
    int idx = 0;
    int pad = 0;

    if (v == 0) tmp[idx++] = '0';
    while (v) {
        int d = (int)(v % (unsigned long)base);
        if (d < 10) tmp[idx++] = (char)('0' + d);
        else tmp[idx++] = (char)((upper ? 'A' : 'a') + (d - 10));
        v = v / (unsigned long)base;
    }
    if (neg) tmp[idx++] = '-';

    pad = width - idx;
    if (pad > 0) outpad(o, pad, pad_zero ? '0' : ' ');
    while (idx--) outch(o, tmp[idx]);
}

static int mini_vformat(struct OutBuf *o, const char *fmt, va_list ap) {
    const char *p = fmt;
    while (*p) {
        if (*p != '%') {
            outch(o, *p++);
            continue;
        }
        p++;
        if (*p == '%') { outch(o, '%'); p++; continue; }

        int pad_zero = 0;
        int width = 0;
        int width_from_arg = 0;
        int precision = -1;
        int prec_from_arg = 0;
        int longflag = 0;

        if (*p == '0') { pad_zero = 1; p++; }

        if (*p == '*') {
            width_from_arg = 1;
            p++;
        } else {
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }
        }

        if (*p == '.') {
            p++;
            precision = 0;
            if (*p == '*') {
                prec_from_arg = 1;
                p++;
            } else {
                while (*p >= '0' && *p <= '9') {
                    precision = precision * 10 + (*p - '0');
                    p++;
                }
            }
        }

        if (*p == 'l') { longflag = 1; p++; }

        if (width_from_arg) width = va_arg(ap, int);
        if (prec_from_arg) precision = va_arg(ap, int);

        switch (*p) {
            case 'd': {
                long v = longflag ? va_arg(ap, long) : va_arg(ap, int);
                unsigned long uv = (v < 0) ? (unsigned long)(-v) : (unsigned long)v;
                outnum(o, uv, 10, 0, width, pad_zero, v < 0);
                break;
            }
            case 'u': {
                unsigned long v = longflag ? va_arg(ap, unsigned long) : (unsigned long)va_arg(ap, unsigned int);
                outnum(o, v, 10, 0, width, pad_zero, 0);
                break;
            }
            case 'x':
            case 'X': {
                unsigned long v = longflag ? va_arg(ap, unsigned long) : (unsigned long)va_arg(ap, unsigned int);
                outnum(o, v, 16, *p == 'X', width, pad_zero, 0);
                break;
            }
            case 'c': {
                int ch = va_arg(ap, int);
                outch(o, ch);
                break;
            }
            case 's': {
                const char *s = va_arg(ap, const char *);
                int len = (int)strlen(s ? s : "(null)");
                int n = (precision >= 0 && precision < len) ? precision : len;
                if (width > n) outpad(o, width - n, ' ');
                outstr(o, s, n);
                break;
            }
            default:
                outch(o, '%');
                outch(o, *p);
                break;
        }
        p++;
    }
    return 0;
}

int vfprintf(FILE *fp, const char *fmt, va_list ap) {
    struct OutBuf o;
    (void)fp;
    o.buf = NULL;
    o.idx = 0;
    o.to_buf = 0;
    mini_vformat(&o, fmt, ap);
    return 0;
}

int fprintf(FILE *fp, const char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    return 0;
}

int printf(const char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    return 0;
}

int sprintf(char *buf, const char *fmt) {
    va_list ap;
    struct OutBuf o;
    va_start(ap, fmt);
    o.buf = buf;
    o.idx = 0;
    o.to_buf = 1;
    mini_vformat(&o, fmt, ap);
    o.buf[o.idx] = '\0';
    va_end(ap);
    return o.idx;
}

int __va_arg(va_list *ap) {
    int v = *(int *)(*ap);
    *ap += 2;
    return v;
}
