#ifndef INCLUDE_STDIO_H_
#define INCLUDE_STDIO_H_

// kernel print use UART
int printk(const char *fmt, ...);

// user print use screen buffer
int kprintk(const char *fmt, ...);
#endif
