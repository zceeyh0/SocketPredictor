// Client header for socket (Multithread version)

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class myTCPClient {
    int m_sockfd;

public:
    myTCPClient();
    ~myTCPClient();
    void connect2Server(const char* ipAddr, const int port);
    int sendMsg(const void* buff, const int buffLen);
    int recvMsg(void* buff, const int buffLen);
};

void* pth_main(void* arg);
