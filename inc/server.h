// Server program for socket (Multithread version)

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
#include <iostream>
#include <vector>

//#include <Python.h>

class myTCPServer {
public:
    int m_listenfd; // socket for listen
    std::vector<int> m_clientfd; // group of sockets for connections

    myTCPServer();
    ~myTCPServer();

    void initServer(int port); // socket, bind and listen for initialising the server
    int openConnection(); // open connection to accept connections from clients
    void closeListen();
    void closeClient();
};

void EXIT(int sig); // function that deals with interruption signals

void* pth_main(void* arg); // main function for communication in a thread
