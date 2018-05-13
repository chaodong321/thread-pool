.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall
LIBS = -lpthread

#DEBUG = -D MYDEBUG_DD
DEBUG = 

SERVER = tcp-proxy
SERVER_SRCS = util-log.o util-coredump.o util-pidfile.o util-cmdline.o util-daemon.o thread-pool.o tcp-server.o tcp-proxy.o

CLIENT = tcp-client
CLIENT_SRCS = tcp-client.o

all: $(SERVER) $(CLIENT)

$(SERVER):$(SERVER_SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(DEBUG)

$(CLIENT):$(CLIENT_SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(DEBUG)

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@ $(DEBUG)

clean:
	rm $(SERVER) $(SERVER_SRCS) $(CLIENT) $(CLIENT_SRCS)
