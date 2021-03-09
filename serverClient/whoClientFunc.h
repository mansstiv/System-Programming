#include "helpFunc.h"

// Thread function 
void * thread_whoClient(void * params);

// Parameters of thread function
struct thread_whoClient_Params {
    char * serverIP;
    int serverPort;
    char * command;
    int numThreads;
    int bytesOfCommand;
};

void initialize_thread_whoClient_Params(char * serverIP, int serverPort, char * command, int numThreads,
                                        int bytes, struct thread_whoClient_Params * params);