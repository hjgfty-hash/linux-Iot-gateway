#include "uart.h"
#include "mqtt.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void* uart_thread(void* arg) {
    char* port = "/dev/pts/2"; // 请确保串口号正确
    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open uart failed");
        return NULL;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    printf("[UART] %s init success, listening...\n", port);

    char buf[256];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf) - 1);
        if (len > 0) {
            printf("[UART] Received: %s\n", buf);
            mqtt_publish_data("server/uart", len, buf);
        } else {
            printf("UART read error, thread exit.\n");
            break;
        }
    }
    close(fd);
    return NULL;
}
