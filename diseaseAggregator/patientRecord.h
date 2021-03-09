#include "headers.h"

extern int bucketSize;
extern int diseaseHashtableNumOfEntries;
////////////////////////////////////////////////////////////
extern volatile sig_atomic_t worker_sigint_sigquit;
extern volatile sig_atomic_t worker_sigusr1;
////////////////////////////////////////////////////////////
extern volatile sig_atomic_t parent_sigint_sigquit;
extern volatile sig_atomic_t parent_sigchld;
extern volatile sig_atomic_t parent_sigusr1;


struct patientRecord
{
	int recordID;
	int age;
	char * patientFirstName;
	char * patientLastName;
	char * diseaseID;
	char * country;
	char * entryDate;
	char * exitDate;
};

typedef struct patientRecord patientRecord;

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

patientRecord * initializePatientRecord(int recordID, int age, char * patientFirstName, char * patientLastName, 
										char * diseaseID, char * country, char * entryDate, char * exitDate);

int compareDates(char * date1, char * date2);

void deletePatientRecord(patientRecord *);

void printRecord(patientRecord * patient);
