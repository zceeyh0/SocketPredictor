#include "server.h"

myTCPServer TCPServer;

void EXIT(int sig) {
    if (sig == 2) {
        printf("\n");
    }
    printf("Received signal %d，program ends.\n", sig);
	// release resources (sockets) before exit
	TCPServer.closeListen();
    TCPServer.closeClient();
    exit(0);
}

int main() {
    for (int ii = 0; ii < 50; ++ii) { // ignore all signals at first
        signal(ii, SIG_IGN);
    }
	// deal with signal 2 (Ctrl+c) and 15 (kill/killall)
	signal(SIGINT, EXIT);
    signal(SIGTERM, EXIT);
    TCPServer.initServer(5051); // socket, bind, listen
    while (true) {
		// the main progress only accepts and creates new threads
		int idx = TCPServer.openConnection();
        pthread_t pthid;
        if (pthread_create(&pthid, NULL, pth_main, (void*)((long)TCPServer.m_clientfd.back())) != 0) {
            perror("pthread_create");
            return -1;
        }
        pthread_detach(pthid); // detach to prevent from zombie threads
        printf("Successfully create a thread for client No.%d\n", idx);
    }
}

