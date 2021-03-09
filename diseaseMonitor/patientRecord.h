#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern int count;

struct patientRecord
{
	int recordID;
	char * patientFirstName;
	char * patientLastName;
	char * diseaseID;
	char * country;
	char * entryDate;
	char * exitDate;
};

typedef struct patientRecord patientRecord;

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

patientRecord * initializePatientRecord(int recordID, char * patientFirstName, char * patientLastName, 
										char * diseaseID, char * country, char * entryDate, char * exitDate);

int compareDates(char * date1, char * date2);

void deletePatientRecord(patientRecord *);

void printRecord(patientRecord * patient);
