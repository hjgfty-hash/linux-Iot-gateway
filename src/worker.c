#include "worker.h"
#include "net.h"
#include "db.h"
#include "mqtt.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

void* work(void* arg) {
    struct PthreadData *pd = (struct PthreadData*)arg;
    int epfd = pd->epfd;
    struct epoll_event events[10];
    
    while(1) {
        int n = epoll_wait(epfd, events, 10, -1);
        for(int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            
            if(fd == pd->efd) {
                uint64_t conn_fd;
                read(fd, &conn_fd, sizeof(conn_fd));
                struct epoll_event ev;
                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);
                continue;
            }

            uint32_t net_len = 0;
            int read_bytes = read_full(fd, &net_len, 4);
            if (read_bytes <= 0) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                continue;
            }

            if (strncmp((char*)&net_len, "GET ", 4) == 0) {
                char http_buf[4096] = {0};
                read(fd, http_buf, sizeof(http_buf) - 1);
                const char *html_body = "<html><body><h1>Hello, My HTTP Server!</h1></body></html>";
                char http_response[512] = {0};
                snprintf(http_response, sizeof(http_response),
                    "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s",
                    strlen(html_body), html_body);
                write_full(fd, (void*)http_response, strlen(http_response));
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                continue; 
            }

            
            int host_len = ntohl(net_len);
            if(host_len == 0 || host_len > 4096){
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                continue;
            }   
            char buf[4096]={0};
            read_full(fd, buf, host_len);
            
            if(strncmp(buf, "save ", 5) == 0){
                int id; char name[20];
                if(sscanf(buf+5, "%d %s", &id, name) == 2){
                    int ret = db_insert_student(id, name);
                    if(ret != SQLITE_OK){
                        uint32_t err_len = htonl(12);
                        write_full(fd, &err_len, 4);
                        write_full(fd, "Save Failed\n", 12);
                    }else{
                        uint32_t ok_len = htonl(13);
                        write_full(fd, &ok_len, 4);
                        write_full(fd, "Save Success\n", 13);
                    }
                }else{
                    uint32_t err_len = htonl(15);
                    write_full(fd, &err_len, 4);
                    write_full(fd, "Format Error!\n", 15);
                }
            }else if(strncmp(buf, "get", 3) == 0){
                char total_buf[4096] = {0};
                db_query_all(total_buf, sizeof(total_buf));
                int total_len = strlen(total_buf);
                if(total_len > 0){
                    uint32_t len = htonl(total_len);
                    write_full(fd, &len, 4);
                    write_full(fd, total_buf, total_len);
                }else{
                    char *empty = "DATABASE IS EMPTY\n";
                    uint32_t len = htonl(strlen(empty));
                    write_full(fd, &len, 4);
                    write_full(fd, empty, strlen(empty));
                }   
            }else{
                uint32_t len = htonl(host_len);
                write_full(fd, &len, 4);
                write_full(fd, buf, host_len);
            }
            
            mqtt_publish_data("server/data", host_len, buf);
        }
    }
    return NULL;
}
