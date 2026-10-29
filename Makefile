CC = gcc

CFLAGS = -Wall -g -Iinclude -pthread

LIBS = -lsqlite3 -lmosquitto
TARGET = iot_server
SRCS = src/main.c src/net.c src/db.c src/mqtt.c src/uart.c src/worker.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
