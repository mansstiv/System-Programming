#include "helpFunc.h"

                            
int get_port(int sockfd);

// Main function called when worker accepts connection by whoServer
void worker_server(int newsock, int bufferSize, int fd[2], struct subdirectory_list * subdirectory_list, int diseaseHashtableNumOfEntries
            , char * new_string, char * serverIP, char * bufferPatientFirstName, char * bufferPatientLastName, 
                char * bufferDiseaseID, char * bufferStatus);

//////////////////////////////////////////// Main Functions ////////////////////////////////////////////

void listCountriesWorker (char ** answer, struct subdirectory_list * subdirectory_list, int * success);

void diseaseFrequencyWorker (char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail);

void topkAgeRangesWorker (char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail);

void searchPatientRecordWorker (int recordID, struct subdirectory_list * subdirectory_list, char ** answer, 
                                int * success, int * fail);

void numPatientAdmissionsWorker (char * command, struct subdirectory_list * subdirectory_list, 
                                int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail);                            

void numPatientDischargesWorker (char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail);

//////////////////////////////////////////// Signals ////////////////////////////////////////////

void workerhandler_sigint_sigquit(int signo);

void workerhandler_sigusr1(int signo);


