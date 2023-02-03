#include "client.h"

myTCPClient TCPClient;
pthread_mutex_t mutex; // declare a mutex lock

void* pth_main(void* arg) { // multithread communication function for client
	pthread_mutex_lock(&mutex); // lock before communication
	int pid = (long)arg;
    printf("\nWelcome to Sunspots Predictor! Technician No.%d is at your service.\n", pid);
    printf("Please input any month in the future ending with the form: YYYY-MM\n");
	printf("For example: Please tell me how many sunspots will be on the sun in 2025-06\n");
    char buffer[1024];
	while (true) {
        memset(buffer, 0, sizeof(buffer));
		// if the input string is shorter than 1024, there will be a '\n' at the end
        fgets(buffer, sizeof(buffer), stdin);
        if (TCPClient.sendMsg(buffer, strlen(buffer)) <= 0) {
            printf("Error when sending message, program ends!\n");
			break;
        }
		printf("Predicting... (It may take longer for the first time)\n");

        memset(buffer, 0, sizeof(buffer));
        if (TCPClient.recvMsg(buffer, sizeof(buffer)) <= 0) {
            printf("Error when receiving message, program ends!\n");
            break;
        }
        printf("Received from the server:\n%s\n", buffer);
		printf("Do you wish to predict again? (y/n):\n");
		char choice;
		scanf("%c", &choice);
		getchar(); // read the '\n' left in the buffer
		if (choice == 'y') {
			printf("Please input another month (YYYY-MM):\n");
			continue;
		} else {
			printf("Thanks for using me. Bye!\n");
			break;
		}
    }
	pthread_mutex_unlock(&mutex); // unlock when communication ends
	usleep(1000); // sleep 1ms for other threads to compete
    pthread_exit(0);
}

int main() {
	TCPClient.connect2Server("192.168.137.5", 5051);
	pthread_mutex_init(&mutex, 0); // initialise the mutex lock
    pthread_t pthid;
	pthread_t pthid2;
	// pthread_t pthid3;
	pthread_create(&pthid, NULL, pth_main, (void*)1);
	pthread_create(&pthid2, NULL, pth_main, (void*)2);
	// pthread_create(&pthid3, NULL, pth_main, (void*)3);
	// three threads are working in parallel, the main progress should wait for their ends
	pthread_join(pthid, NULL);
	pthread_join(pthid2, NULL);
	// pthread_join(pthid3, NULL);
	// pthread_mutex_destroy(&mutex); // destroy the mutex lock
    return 0;
}

