#include "net.h"
#include<unistd.h>
int read_full(int fd,void *buf,size_t count){
        int total=0;
        while(total<count){
                int n=read(fd,(char*)buf+total,count-total);
                if(n<=0) return n;
                total+=n;
        }
        return total;
}


int write_full(int fd,void *buf,size_t count){
        int total=0;
        while(total<count){
                int n=write(fd,(char*)buf+total,count-total);
                if(n<=0) return n;
                total+=n;
        }
        return total;
}
