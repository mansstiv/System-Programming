#include <stdio.h>
#include <stdlib.h>

#include "mainFunc.h"


int main(int argc, char *argv[]) 
{

	char * patientRecordsFile = NULL;
	char *buffer = NULL; // Temp buffer for commands of user
	int diseaseHashtableNumOfEntries = -1, countryHashtableNumOfEntries = -1, bucketSize = -1, caseNum;


	for (int i=0; i<argc; i++)
	{
		if (strcmp(argv[i], "-p") == 0) 
			patientRecordsFile = argv[i+1];
		else if (strcmp(argv[i], "-h1") == 0)
			diseaseHashtableNumOfEntries = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-h2") == 0)
			countryHashtableNumOfEntries = atoi(argv[i+1]);		
		else if (strcmp(argv[i], "-b") == 0)
			bucketSize = atoi(argv[i+1]);
	}

	if (patientRecordsFile == NULL || diseaseHashtableNumOfEntries == -1 || countryHashtableNumOfEntries == -1 || bucketSize == -1)
	{
		printf("Error: Not valid arguements\n");
		exit(1);
	}

	struct list * list = new_list();
	struct bucket ** diseaseHashtable;
	struct bucket ** countryHashtable;

	diseaseHashtable = malloc(sizeof(struct bucket *) * diseaseHashtableNumOfEntries);
	countryHashtable = malloc(sizeof(struct bucket *) * countryHashtableNumOfEntries);

	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		diseaseHashtable[i] = NULL;

	for (int i = 0; i < countryHashtableNumOfEntries; ++i)
		countryHashtable[i] = NULL;

	insert_to_structs(patientRecordsFile, list, bucketSize, diseaseHashtable, diseaseHashtableNumOfEntries, 
						countryHashtable, countryHashtableNumOfEntries);


	while (1)
	{

		printf("1. /globalDiseaseStats  [date1 date2]\n");
		printf("2. /diseaseFrequency virusName [country] date1 date2\n");
		printf("3. /topk-Diseases k country  [date1 date2]\n");
		printf("4. /topk-Countries k disease [date1 date2]\n");			
		printf("5. /insertPatientRecord recordID patientFirstName patientLastName diseaseID country entryDate [exitDate]\n");
		printf("6. /recordPatientExit recordID exitDate\n");
		printf("7. /numCurrentPatients [disease]\n");
		printf("8. /exit\n\n");
		printf("COMMAND: ");
	
		buffer = malloc(sizeof(char) * 1000);
        
        if (fgets(buffer, 1000, stdin) == NULL) // Added this line of code for validator to work
        	return 1;							// Return when you find EOF

        buffer[strlen(buffer) - 1] = '\0';		
        caseNum = get_case_num(buffer);

        switch (caseNum)
        {
        	case -1:
        		printf("\n");
        		printf("Wrong command, type again!\n");
        		break;
        	
        	case 1:	
       			printf("\n");
        		globalDiseaseStats(buffer, diseaseHashtable, diseaseHashtableNumOfEntries);
        		break;
        	
        	case 2:	
        		printf("\n");
        		diseaseFrequency(buffer, diseaseHashtable, diseaseHashtableNumOfEntries);
        		break;

        	case 3:
        		printf("\n");
        		topkDiseases(buffer, countryHashtable, countryHashtableNumOfEntries);
        		break;

        	case 4:
        		printf("\n");
        		topkCountries(buffer, diseaseHashtable, diseaseHashtableNumOfEntries);
        		break;

        	case 5:
        		printf("\n");
        		insertPatientRecord(buffer, diseaseHashtable, diseaseHashtableNumOfEntries, countryHashtable, 
        						countryHashtableNumOfEntries, list, bucketSize);
        		break;

        	case 6:
        		printf("\n");
        		recordPatientExit(buffer, list);
        		break;

        	case 7:
        		printf("\n");
        		numCurrentPatients(buffer, diseaseHashtable, diseaseHashtableNumOfEntries);
        		break;

        	case 8:
        		printf("exiting\n");
        		free(buffer);
        		delete_hashtables(diseaseHashtable, diseaseHashtableNumOfEntries, countryHashtable, countryHashtableNumOfEntries);
				delete_list(list);
				free(list);
        		
        		return 1;

        }

        printf("\n");
        free(buffer);        
	}
}	