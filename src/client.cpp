// Client program for socket (Multithread version)

#include "client.h"

myTCPClient::myTCPClient() {
    m_sockfd = 0;
}

myTCPClient::~myTCPClient() {
    if (m_sockfd != 0) {
        close(m_sockfd);
    }
}

void myTCPClient::connect2Server(const char* ipAddr, const int port) {
	// 1. create a socket (sockfd)
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
        perror("socket");
        exit(0);
    }
	
	// 2. request connecting to the indicated server
	struct hostent* h;
    if ((h = gethostbyname(ipAddr)) == 0) {
        perror("gethostbyname");
        close(m_sockfd);
        exit(0);
    }
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
    if (connect(m_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("connect");
        close(m_sockfd);
        exit(0);
    }
}

int myTCPClient::sendMsg(const void* buff, const int buffLen) {
    return send(m_sockfd, buff, buffLen, 0);
}

int myTCPClient::recvMsg(void* buff, const int buffLen) {
    return recv(m_sockfd, buff, buffLen, 0);
}

