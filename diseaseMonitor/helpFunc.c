#include "helpFunc.h"

int count;

unsigned int hash(char *str, unsigned int HASHSIZE)	// hash function by Dan Bernstein 
														// inspired by http://www.cs.yorku.ca/~oz/hash.html
{
    unsigned int hashval;

    for (hashval = 0; *(str) != '\0'; str++)
        hashval = *str + 31*hashval;
 
    return hashval % HASHSIZE;
}

void insert_to_structs(char * patientRecordsFile, struct list * list, int bucketSize, struct bucket ** diseaseHashtable, 
						int diseaseHashtableNumOfEntries, struct bucket ** countryHashtable, int countryHashtableNumOfEntries)
{
	int numOfPatients = 0;
	char c;
 	
	FILE *fp = fopen (patientRecordsFile, "r");

	// Counting sum of patients
   	while (1)
   	{
   		c = fgetc(fp);
   
    	if (feof(fp)) 
        	break ;	

    	if (c=='\n')
        	numOfPatients++;   
    }
	
    fclose(fp);

    // Variables used in main for loop
    patientRecord *tempPatient;
	struct lnode *tempLnode;	
    int bufferRecordID, hashval, error;
	char * bufferPatientFirstName,  * bufferPatientLastName, * bufferDiseaseID, 
			* bufferCountry, * bufferEntryDate, * bufferExitDate;	

	fp = fopen (patientRecordsFile, "r");

    // Main loop for reading patients records and initializing structs 
    for (int i = 0; i < numOfPatients; ++i)
    {
    	error = 0; // equals to 1 when error in date or when duplicate 

		// Allocating space for temp buffers
		bufferPatientFirstName = malloc(sizeof(char) * 100);
		bufferPatientLastName = malloc(sizeof(char) * 100);
 		bufferDiseaseID = malloc(sizeof(char) * 100);
		bufferCountry = malloc(sizeof(char) * 100);
		bufferEntryDate = malloc(sizeof(char) * 100);
		bufferExitDate = malloc(sizeof(char) * 100);

		fscanf(fp, "%d %s %s %s %s %s %s", &bufferRecordID, bufferPatientFirstName, bufferPatientLastName, bufferDiseaseID, 
											bufferCountry, bufferEntryDate, bufferExitDate);

		tempPatient = initializePatientRecord(bufferRecordID, bufferPatientFirstName, bufferPatientLastName, 
								bufferDiseaseID, bufferCountry, bufferEntryDate, bufferExitDate);

		tempLnode = new_lnode(tempPatient);

		// entryDate must be <= exitDate

		if (strcmp(bufferExitDate, "-") != 0 && compareDates(bufferEntryDate, bufferExitDate) > 0)
		{
			printf("Error in patient: ");
			printRecord(tempPatient);
			printf("Not valid exitDate \n");
			error = 1;
		}

		// Check for duplicate
		if (error == 0)
		{
			if (find_patient(list, bufferRecordID) == NULL)
				insert_lnode(list, tempLnode);
			else
			{
				error = 1;
				printf("Error in patient: ");
				printRecord(tempPatient);
				printf("RecordID already exists\n");

			}		
		}

		// Error occured so exit 
		if (error == 1) 
		{
			// Deleting space of temp buffers
			free(bufferPatientFirstName);	
			free(bufferPatientLastName);	
			free(bufferDiseaseID);
			free(bufferCountry);
			free(bufferEntryDate);
			free(bufferExitDate);

			// Delete space of structs being used for invalid patient record  
			deletePatientRecord(tempPatient);
			free(tempLnode);

			fclose(fp);

			return;
		}

	// Insert to diseaseHashtable 

		hashval = hash(tempPatient->diseaseID, diseaseHashtableNumOfEntries);

		// No buckets in current position of diseaseHashtable
		if (diseaseHashtable[hashval] == NULL)
			diseaseHashtable[hashval] = new_bucket(bucketSize);
		
		insert_patient_to_bucket(diseaseHashtable[hashval], tempPatient, bucketSize , 0);

	// Insert to countryHashtable 

		hashval = hash(tempPatient->country, countryHashtableNumOfEntries);

		// No buckets in current position of countryHashtable
		if (countryHashtable[hashval] == NULL)
			countryHashtable[hashval] = new_bucket(bucketSize);
		
		insert_patient_to_bucket(countryHashtable[hashval], tempPatient, bucketSize , 1);		
			

		// Deleting space for temp buffers
		free(bufferPatientFirstName);	
		free(bufferPatientLastName);	
		free(bufferDiseaseID);
		free(bufferCountry);
		free(bufferEntryDate);
		free(bufferExitDate);
    }

    fclose(fp);
}

void uber_print(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;
	int bucket_count = 0;

	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
	{
		bucket_count = 0;
		printf("///////////// index = %d /////////////\n", i);

		if (diseaseHashtable[i] != NULL)
		{
			tempBucket = diseaseHashtable[i];
			
			while (tempBucket != NULL)
			{
				bucket_count++;
				printf("///////////// bucket_count = %d, numOfEntries = %d /////////////\n", bucket_count, tempBucket->numOfEntries);

				for (int j = 0; j < tempBucket->numOfEntries; ++j)
				{	
					memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
					printf("||||||  %s  ||||||\n", tempBucketEntry.key);
					postorder(tempBucketEntry.tree, tempBucketEntry.tree->root);	
				}

				tempBucket = tempBucket->next;
			}
		}
	}
}

void delete_hashtables(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries,
						struct bucket ** countryHashtable, int countryHashtableNumOfEntries)
{

	struct bucket * prevBucket;
	struct bucket * tempBucket;
	struct bucketEntry * tempBucketEntry;

// Deleting diseaseHashtable
	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
	{

		if (diseaseHashtable[i] != NULL)
		{
			tempBucket = diseaseHashtable[i];
			prevBucket = tempBucket;

			while (tempBucket != NULL)
			{
				tempBucket = tempBucket->next;

				for (int j = 0; j < prevBucket->numOfEntries; ++j)
				{
					tempBucketEntry = prevBucket->bytes + j * sizeof(struct bucketEntry); 

					free(tempBucketEntry->key);
					destroyRBTree(tempBucketEntry->tree, tempBucketEntry->tree->root);
					free(tempBucketEntry->tree);
				}

				free(prevBucket->bytes);
				free(prevBucket);

				prevBucket = tempBucket;

			}
		}
	}

// Deleting countryHashtable
	for (int i = 0; i < countryHashtableNumOfEntries; ++i)
	{

		if (countryHashtable[i] != NULL)
		{
			tempBucket = countryHashtable[i];
			prevBucket = tempBucket;

			while (tempBucket != NULL)
			{
				tempBucket = tempBucket->next;

				for (int j = 0; j < prevBucket->numOfEntries; ++j)
				{
					tempBucketEntry = prevBucket->bytes + j * sizeof(struct bucketEntry); 

					free(tempBucketEntry->key);
					destroyRBTree(tempBucketEntry->tree, tempBucketEntry->tree->root);
					free(tempBucketEntry->tree);
				}

				free(prevBucket->bytes);
				free(prevBucket);

				prevBucket = tempBucket;

			}
		}
	}	

	free(diseaseHashtable);
	free(countryHashtable);
}

int get_case_num(char * buffer)
{
	char temp_str[100];
	strcpy(temp_str, buffer);
	char *temp = strtok(temp_str, " ");

	if (strcmp(temp, "/globalDiseaseStats") == 0)
		return 1;
	else if (strcmp(temp, "/diseaseFrequency") == 0)
		return 2;
	else if (strcmp(temp, "/topk-Diseases") == 0)
		return 3;
	else if (strcmp(temp, "/topk-Countries") == 0)
		return 4;
	else if (strcmp(temp, "/insertPatientRecord") == 0)
		return 5;
	else if (strcmp(temp, "/recordPatientExit") == 0)
		return 6;
	else if (strcmp(temp, "/numCurrentPatients") == 0)
		return 7;
	else if (strcmp(temp, "/exit") == 0)
		return 8;
	else 
		return -1;
}

