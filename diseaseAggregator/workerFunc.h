#include "helpFunc.h"

                            

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


