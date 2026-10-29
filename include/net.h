#ifndef NET_H
#define NET_H
#include <stddef.h>
int read_full(int fd,void *buf,size_t count);
int write_full(int fd,void *buf,size_t count);
#endif
