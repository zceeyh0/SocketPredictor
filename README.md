# SocketPredictor
* The program is able to build an MLP machine learning model to perform time-series predictions. <br>
* The dataset being used is the Sunspots dataset which contains the numbers of sunspots in 235 years (1749 to 1983). <br>
* The socket programs for server and client are written in C++. The MLP predictor is written in Python. <br>

## Organisation of files and their roles:

#### 1. `README.md`: <br>
Introduction to this repository with instructions on how to run the program.<br>
#### 2. `Sunspots.csv`: <br>
This CSV file contains the dataset of the number of sunspots from 1749 to 1983.<br>
#### 3. `src/mlpPredictor.py`: <br>
This python file implements the MLP model in a class to return prediction results and plot graphs for the predictions. The performance of the MLP model can be evaluated using the main function in this file.<br>
#### 4. `inc/server.h, src/server.cpp, src/serverMain.cpp`: <br>
These three files build up a multithreading socket server for predictions. The communication with a single client will be processed in a separate thread once the connection is built.<br>
#### 5. `inc/client.h, src/client.cpp, src/clientMain.cpp`: <br>
These three files build a multithreading socket client to connect with the server. It performs as a chatbot to ask for a sentence from the user and get the date for a prediction. After the prediction is done, the client will receive the result from the server and display it on the user's terminal.<br>

## The necessary Python packages/modules are: <br>
`numpy 1.20.3`, `pandas 1.3.4`, `matplotlib 3.5.0`, `tensorflow 2.7.0` <br>
When downloading the packages/modules above, other essential packages/modules will be downloaded automatically as dependencies.

## To run the program: 
Simply execute the binary file `serverMain` on the server terminal and execute the binary file `clientMain` on one or more client terminals. Please make sure you are running the program in Linux. <br>
## If you want to make some changes, the compilation steps are: <br>
1. Run command `g++ -fPIC -shared -o lib/libserver.so src/server.cpp -I inc` to compile the dynamic library for the server. <br>
2. Run command `g++ -fPIC -shared -o lib/libclient.so src/client.cpp -I inc` to compile the dynamic library for the client.
3. Run command `g++ -o serverMain src/serverMain.cpp -I inc -L lib -l server -l pthread` to compile the server main program (make sure you include the pthread library).
4. Run command `g++ -o clientMain src/clientMain.cpp -I inc -L lib -l client -l pthread` to compile the client main program.
5. If the program cannot be executed, make sure you set the path for dynamic libraries by `export LD_LIBRARY_PATH=lib:.`.

# Enjoy!
