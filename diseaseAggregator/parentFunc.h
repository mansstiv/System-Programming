#include "helpFunc.h"


void listCountries (char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2]);

void diseaseFrequency(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
						char ** countries_path, int numCountries, int * success, int * fail);

void topkAgeRanges(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
							char ** countries_path, int numCountries, int * success, int * fail);

void searchPatientRecord(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2]);

void numPatientAdmissions(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
						char ** countries_path, int numCountries, int * success, int * fail);

void numPatientDischarges(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
							char ** countries_path, int numCountries, int * success, int * fail);


/////////////////////////////////////////////

void parenthandler_sigint_sigquit(int signo);

void parenthandler_sigchld(int signo);

void parenthandler_sigusr1(int signo);

void exit_with_sigint_sigquit(char ** countries_path, int countries_count, int numWorkers, int workers_pids[numWorkers],
								int * success, int * fail);

void sigchld_function(int numWorkers, int workers_pids[numWorkers], char ** countries_path_for_workers, int * worker_bytes,
						int fd[numWorkers][2], int bufferSize);

void sigurs1_function(int numWorkers, int fd[numWorkers][2], int bufferSize);
////////////////////////////////////////////