// Server program for socket (Multithread version)

#include "server.h"

myTCPServer::myTCPServer() {
	m_listenfd = 0;
	m_clientfd = std::vector<int>();
}

myTCPServer::~myTCPServer() {
	closeListen();
	closeClient();
}

void myTCPServer::initServer(int port) {
	// 1. create a socket (listenfd)
	m_listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (m_listenfd == -1) {
        perror("socket");
        exit(0); 
    }

	// 2. bind the socket represented by listenfd to any IP address and a specific port number
	struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
    if (bind(m_listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("bind");
        closeListen();
        exit(0); 
    }

	// 3. server goes into listen mode
	if (listen(m_listenfd, 5) != 0) {
        perror("listen");
        closeListen();
        exit(0); 
    }
}

int myTCPServer::openConnection() {
    struct sockaddr_in clientaddr; // stores the IP address and port number of the current client
    int socklen = sizeof(struct sockaddr_in);
    printf("Waiting for connection from a client...\n");
	// if "accept" succeeds, push the returned client socket into the vector m_clientfd
    m_clientfd.push_back(accept(m_listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&socklen));
	int index = m_clientfd.size() - 1;
	if (m_clientfd.back() == -1) {
        perror("accept");
		m_clientfd.pop_back();
        closeListen();
		closeClient();
        exit(0);
    } else {
        printf("Client No.%d is connected, IP(%s).\n", index, inet_ntoa(clientaddr.sin_addr));
    	return index;
	}
}

void myTCPServer::closeListen() {
	if (m_listenfd != 0) {
		close(m_listenfd);
	}
}

void myTCPServer::closeClient() {
	while (!m_clientfd.empty()) { // close sockets that are not NULL while clearing the vector
		if (m_clientfd.back() != 0) {
			close(m_clientfd.back());
		}
		m_clientfd.pop_back();
	}
}

void* pth_main(void* arg) {
	// 客户端socket一定要在创建线程时用参数传入，否则在线程未创建完毕时，栈顶可能变更（新的accept）
	// 传入的socket被事先强转成long（8字节）后转成void*（8字节），因此使用前需要转回long再赋给int
	int clientfd = (long)arg;
	
	char buffer[1024];
    while (true) {
		memset(buffer, 0, sizeof(buffer));
    	if (recv(clientfd, buffer, sizeof(buffer), 0) <= 0) {
            break;
    	}
		int len = strlen(buffer);
		if (len < 8) {	
			memset(buffer, 0, sizeof(buffer));
    		sprintf(buffer, "Your input is too short!\n");
    		if (send(clientfd, buffer, strlen(buffer), 0) <= 0) {
            	break;
    		}
		}
		// get the last 8 characters for prediction (YYYY-MM\0)
		char curMonth[8] = {0};
		for (int i = len - 8; i < len; ++i) {
			curMonth[i - len + 8] = buffer[i];
		}
		// calculate the value of input year and month
		int year = 0;
		for (int i = 0; i < 4; ++i) {
			year = year * 10 + (curMonth[i] - '0');
		}
		if (year < 1749) {
			memset(buffer, 0, sizeof(buffer));
    		sprintf(buffer, "Your input year is too early!\n");
    		if (send(clientfd, buffer, strlen(buffer), 0) <= 0) {
            	break;
    		}
		}
		if (year > 2262) {
			memset(buffer, 0, sizeof(buffer));
    		sprintf(buffer, "Your input year is too late!\n");
    		if (send(clientfd, buffer, strlen(buffer), 0) <= 0) {
            	break;
    		}
		}
		int month = (curMonth[5] - '0') * 10 + (curMonth[6] - '0');
		if (month < 1 || month > 12) {
			memset(buffer, 0, sizeof(buffer));
    		sprintf(buffer, "Your input month is not correct!\n");
    		if (send(clientfd, buffer, strlen(buffer), 0) <= 0) {
            	break;
    		}
		}
    	printf("Predicting for %s", curMonth);

		// 1. Initialise the interface for python interpreter
		Py_Initialize();
		if (!Py_IsInitialized()) {
			printf("Python interpreter initialisation failed\n");
			break;
		}
		
		// 2. Import python system files and append source codes
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('./src')");
    	
		PyObject* pModule = nullptr;
		// 3. Get the module by filename
		pModule = PyImport_ImportModule("mlpPredictor");
		// 4. Get the training set from dataset by calling get_xtrain()
		PyObject* pFuncOne = nullptr;
		pFuncOne = PyObject_GetAttrString(pModule, "get_xtrain");
		PyObject* pArg = PyTuple_New(1);
		PyTuple_SetItem(pArg, 0, Py_BuildValue("s", "../Sunspots.csv"));
		PyObject* pReturnX = PyEval_CallObject(pFuncOne, pArg);
		// 5. Get the test set from dataset by calling get_ytrain()
		PyObject* pFuncTwo = nullptr;
		pFuncTwo = PyObject_GetAttrString(pModule, "get_ytrain");
		PyObject* pReturnY = PyEval_CallObject(pFuncTwo, pArg);
		// 6. Predict the result
		PyObject* pFuncThree = nullptr;
		pFuncThree = PyObject_GetAttrString(pModule, "mlp_predict");
		PyObject* pArgs = PyTuple_New(3);
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("p", pReturnX)); // X training set
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("p", pReturnY)); // Y test set
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", (year - 1749) * 12 + month)); // Number of months
		PyObject* pReturnAns = PyEval_CallObject(pFuncThree, pArgs);
		// 7. Transform the predicted result into integer to send back
		int result = 0;
		PyArg_Parse(pReturnAns, "i", &result);
		// 8. Close the python interpreter
		Py_Finalize();
		memset(buffer, 0, sizeof(buffer));
    	sprintf(buffer, "Thanks for your patience. Your predicted result is: %d.", result);
    	if (send(clientfd, buffer, strlen(buffer), 0) <= 0) {
            break;
    	}   
    }
	printf("Lose connection from a client or due to an error\n");
	close(clientfd); // close the current socket
	pthread_exit(0); // pthread_exit instead of return (the latter may end the whole program)
}
