#include "patientRecord.h"

patientRecord * initializePatientRecord(int recordID, int age, char * patientFirstName, char * patientLastName, 
										char * diseaseID, char * country, char * entryDate, char * exitDate)
/* allocate and initialize memory for patientRecord */
{

	patientRecord * patient = malloc(sizeof(patientRecord));

	patient->recordID = recordID;

	patient->age = age;

	patient->patientFirstName = malloc(strlen(patientFirstName) + 1);
	strcpy(patient->patientFirstName, patientFirstName);

	patient->patientLastName = malloc(strlen(patientLastName) + 1);
	strcpy(patient->patientLastName, patientLastName);

	patient->diseaseID = malloc(strlen(diseaseID) + 1);
	strcpy(patient->diseaseID, diseaseID);
	
	patient->country = malloc(strlen(country) + 1);
	strcpy(patient->country, country);

	if (entryDate != NULL)
	{
		patient->entryDate = malloc(strlen(entryDate) + 1);
		strcpy(patient->entryDate, entryDate);
	}
	else
		patient->entryDate = NULL;

	if (exitDate != NULL)
	{
		patient->exitDate = malloc(strlen(exitDate) + 1);
		strcpy(patient->exitDate, exitDate);
	}
	else 
		patient->exitDate = NULL;

	// printf("create || "); printRecord(patient);

	return patient;
}

// returns -1 , if date1 < date2
// returns 0 , if date1 = date2day
// returns 1 ,  if date1 > date2
int compareDates(char * date1 , char * date2)
{	
	//20-12-2010 23-04-2018

	int retval;
	char *tempDate;
	char * day1, * month1, * year1;
	char * day2, * month2, * year2;

	tempDate = malloc(sizeof(char) * 20);
	
	day1 = malloc(sizeof(char) * 10);
	month1 = malloc(sizeof(char) * 10);
	year1 = malloc(sizeof(char) * 10);

	day2 = malloc(sizeof(char) * 10);
	month2 = malloc(sizeof(char) * 10);
	year2 = malloc(sizeof(char) * 10);

	strcpy(tempDate, date1);

	// Parse date1
    char * token = strtok(tempDate, "-"); 
    strcpy(day1, token);
    token = strtok(NULL, "-"); 
    strcpy(month1, token);
    token = strtok(NULL, "-"); 
    strcpy(year1, token);

    strcpy(tempDate, date2);

    // Parse date2
    token = strtok(tempDate, "-"); 
    strcpy(day2, token);
    token = strtok(NULL, "-"); 
    strcpy(month2, token);
    token = strtok(NULL, "-"); 
    strcpy(year2, token);

    // Actual compare of dates
    if (strcmp(year1, year2) < 0)
    	retval = -1;
    else if (strcmp(year1, year2) > 0)
    	retval = 1;
    else // year1 = year2, so compare months
    {
    	if (strcmp(month1, month2) < 0)
    		retval = -1;
    	else if (strcmp(month1, month2) > 0)
    		retval = 1;
    	else // month1 = month2, so compare dates
    	{
    		if (strcmp(day1, day2) < 0)
    			retval = -1;
    		else if (strcmp(day1, day2) > 0)
    			retval = 1;
    		else 
    			retval = 0;
    	}
    }

	free(day1);
	free(day2);
	free(month1);
	free(month2);
	free(year1);
	free(year2);	
	free(tempDate);

	return retval;
}

void deletePatientRecord(patientRecord * patient)
/* free memory allocated for patientRecord */

{
	// printf("delete || "); printRecord(patient);

	free(patient->patientFirstName);
	free(patient->patientLastName);
	free(patient->diseaseID);
	free(patient->country);
	free(patient->entryDate);
	free(patient->exitDate);
	free(patient);
	patient = NULL;
}

void printRecord(patientRecord * patient) 
{
	printf("%d %s %s %s %s %s %s %d\n", patient->recordID, patient->patientFirstName, patient->patientLastName, 
										patient->diseaseID, patient->country, patient->entryDate, 
										patient->exitDate, patient->age);
}
