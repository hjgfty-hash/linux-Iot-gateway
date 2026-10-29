#include <netinet/in.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdint.h>
#include<arpa/inet.h>

int read_full(int fd, void *buf, size_t count) {
    int total = 0;
    while (total < count) {
        int n = read(fd, (char*)buf + total, count - total);
        if (n <= 0) return n;
        total += n;
    }
    return total;
}

int write_full(int fd, const void *buf, size_t count) {
    int total = 0;
    while (total < count) {
        int n = write(fd, (const char*)buf + total, count - total);
        if (n <= 0) return n;
        total += n;
    }
    return total;
}

int main(){
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(8888);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	int clientfd=socket(AF_INET,SOCK_STREAM,0);
	socklen_t addrlen=sizeof(addr);
	if(connect(clientfd,(struct sockaddr*)&addr,addrlen)<0){
		perror("connect failed");
		return -1;
	}
	while(1){
		char buf[1024];
		memset(buf,0,sizeof(buf));
		int n=read(0,buf,sizeof(buf));
		if(n<=0) continue;

		if(buf[n-1]=='\n'){
			buf[n-1]='\0';
			n--;
		}
		if(n==0) continue;
		
		uint32_t net_len=htonl(n);
		write_full(clientfd,&net_len,4);
		write_full(clientfd,buf,n);
		 
               uint32_t recv_net_len;
               if(read_full(clientfd,&recv_net_len,4)>0){
	       int recv_len = ntohl(recv_net_len);
               if (recv_len > 0 && recv_len <= 4096) {
                    char recv_buf[4096] = {0};
                    read_full(clientfd, recv_buf, recv_len);
                    printf("%s\n", recv_buf); 
                }
	}
}
	close(clientfd);
	return 0;
}
