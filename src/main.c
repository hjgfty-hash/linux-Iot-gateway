#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "worker.h"
#include "db.h"
#include "mqtt.h"
#include "uart.h"

#define MAX_EVENTS 10

struct PthreadData worker[4]; 

int main() {
    init_mqtt();
    init_db();

    
    for(int i = 0; i < 4; i++) {
        worker[i].epfd = epoll_create1(0);
        worker[i].efd = eventfd(0, EFD_NONBLOCK);
        
        struct epoll_event ev;
        ev.data.fd = worker[i].efd;
        ev.events = EPOLLIN;
        epoll_ctl(worker[i].epfd, EPOLL_CTL_ADD, worker[i].efd, &ev);   
        pthread_create(&worker[i].tid, NULL, work, (void*)&worker[i]);
        pthread_detach(worker[i].tid);
    }

    pthread_t uart_tid;
    pthread_create(&uart_tid, NULL, uart_thread, NULL);
    pthread_detach(uart_tid);

    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    listen(sockfd, 10);
    
    int epfd=epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    int current_worker = 0;
    while(1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for(int i = 0; i < n; i++) {
            if(events[i].data.fd == sockfd) {
                int conn_fd = accept(sockfd, NULL, NULL);
                if(conn_fd < 0) continue;

                uint64_t val = conn_fd;
                write(worker[current_worker].efd, &val, sizeof(val));
                current_worker = (current_worker + 1) % 4;
            }
        }
    }
    close(sockfd);
    return 0;
}
