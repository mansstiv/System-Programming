#include "mainFunc.h"

void globalDiseaseStats(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{
	count = 0;
	
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	
	char temp_str[100];
	char * temp;

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");

// No [date1 date2] case
	if (temp == NULL)
	{
		for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		{
			if (diseaseHashtable[i] != NULL)
			{
				tempBucket = diseaseHashtable[i];
				
				while (tempBucket != NULL)
				{

					for (int j = 0; j < tempBucket->numOfEntries; ++j)
					{	
						memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
						printf("%s %d\n", tempBucketEntry.key, get_number_of_patients(tempBucketEntry.tree, 
																					tempBucketEntry.tree->root, NULL, NULL, NULL, 0));
						count = 0;
					}

					tempBucket = tempBucket->next;
				}
			}
		}		

		return;
	}

	char * date1;
	char * date2;

	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);

	temp = strtok(NULL, " ");

	// Gave entryDate but not exitDate so exit
	if (temp == NULL) 
	{
		free(date1);
		printf("You didn't give date2 parameter\n");
		return;
	}

// [entryDate1 entryDate2] case
 	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);

	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
	{
		if (diseaseHashtable[i] != NULL)
		{
			tempBucket = diseaseHashtable[i];
	
			while (tempBucket != NULL)
			{
				for (int j = 0; j < tempBucket->numOfEntries; ++j)
				{	
					memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
					printf("%s %d\n", tempBucketEntry.key, get_number_of_patients(tempBucketEntry.tree, 
																				tempBucketEntry.tree->root, date1, date2, NULL, 0));
					count = 0;
				}
			
				tempBucket = tempBucket->next;
			}
		}
	}	

	free(date1);
	free(date2);

}


void diseaseFrequency(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{
	count = 0;
	
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	
	char * virusName;
	char * country = NULL;
	char * date1, * date2;
	char temp_str[100];
	char * temp;

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");	

	// Initilaize virusName
	virusName = malloc(strlen(temp) + 1);
	strcpy(virusName, temp);

	temp = strtok(NULL, " ");
	
	// Check if user gave country
	// Case 1: diseaseFrequency virusName date1 date2
	if (temp[0] >= '0' && temp[0] <= '9')
	{
		// Initialize date1
		date1 = malloc(strlen(temp) + 1);
		strcpy(date1, temp);

		temp = strtok(NULL, " ");

		// Initialize date2
		date2 = malloc(strlen(temp) + 1);
		strcpy(date2, temp);

		for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		{
			if (diseaseHashtable[i] != NULL)
			{
				tempBucket = diseaseHashtable[i];
		
				while (tempBucket != NULL)
				{
					for (int j = 0; j < tempBucket->numOfEntries; ++j)
					{	
						memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
						if (strcmp(tempBucketEntry.key, virusName) == 0)
						{
							printf("%s %d\n", tempBucketEntry.key, get_number_of_patients(tempBucketEntry.tree, 
																				tempBucketEntry.tree->root, date1, date2, NULL, 0));
							count = 0;
						}
					}
				
					tempBucket = tempBucket->next;
				}
			}
		}			
	}	
	// Case 2: diseaseFrequency country virusName date1 date2
	else
	{
		//Initialize country
		country = malloc(strlen(temp) + 1);
		strcpy(country, temp);

		temp = strtok(NULL, " ");

		// Initialize date1
		date1 = malloc(strlen(temp) + 1);
		strcpy(date1, temp);

		temp = strtok(NULL, " ");

		// Initialize date2
		date2 = malloc(strlen(temp) + 1);
		strcpy(date2, temp);		
		
		for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		{
			if (diseaseHashtable[i] != NULL)
			{
				tempBucket = diseaseHashtable[i];
		
				while (tempBucket != NULL)
				{
					for (int j = 0; j < tempBucket->numOfEntries; ++j)
					{	
						memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
						if (strcmp(tempBucketEntry.key, virusName) == 0)
						{
							printf("%s %d\n", tempBucketEntry.key, get_number_of_patients(tempBucketEntry.tree, 
																	tempBucketEntry.tree->root, date1, date2, country, 0));
							count = 0;
						}
					}
				
					tempBucket = tempBucket->next;
				}
			}
		}		
	}


	free(virusName);
	free(date1);
	free(date2);
	if (country != NULL)
		free(country);
}

void topkDiseases(char * buffer, struct bucket ** countryHashtable, int countryHashtableNumOfEntries)
{
	int k;
	char temp_str[100];
	char * temp, * country, * date1, * date2;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	
	struct listCases * list;
	struct tree * heap;

	date1 = NULL;
	date2 = NULL;

	// Initializing k
	strcpy(temp_str, buffer);
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");	
	k = atoi(temp);

	// Initializing country
	temp = strtok(NULL, " ");	
	country = malloc(strlen(temp) + 1);
	strcpy(country, temp);

	temp = strtok(NULL, " ");
	// Checking for date parameters
	if (temp != NULL) 
	{
		date1 = malloc(strlen(temp) + 1);
		strcpy(date1, temp);

		temp = strtok(NULL, " ");
		// Checking for date2 parameter
		if (temp == NULL)
		{
			free(date1);
			free(country);
			date1 = NULL;
			printf("You didn't give date2 parameter\n");
			return ;
		}
		else
		{
			date2 = malloc(strlen(temp) + 1);
			strcpy(date2, temp);
		}
	}

	list = new_listCases();
	heap = new_tree();

	// Traversing buckets and initializing listCases 
	// each node of listCases has a key (diseaseID) and num of cases  
	for (int i = 0; i < countryHashtableNumOfEntries; ++i)
	{
		if (countryHashtable[i] != NULL)
		{
			tempBucket = countryHashtable[i];
	
			while (tempBucket != NULL)
			{
				for (int j = 0; j < tempBucket->numOfEntries; ++j)
				{	
					memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
					
					if (strcmp(tempBucketEntry.key, country) == 0)					
						initialize_listCases(list, tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, "diseaseID");
				}
			
				tempBucket = tempBucket->next;
			}
		}
	}		

	initialize_heap(heap, list);

	int numOfNodes = heap->numOfNodes;
	if (k > numOfNodes)
		k = numOfNodes;

	for (int i = 0; i < k; ++i)
		pop(heap, list);

	// Delete remaining heap
	if (k < numOfNodes)
		delete_heap(list, heap, heap->root);

	free(date1);
	free(date2);
	free(country);
	free(list);
	free(heap);
}

void topkCountries(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{
	int k;
	char temp_str[100];
	char * temp, * disease, * date1, * date2;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	
	struct listCases * list;
	struct tree * heap;

	date1 = NULL;
	date2 = NULL;

	// Initializing k
	strcpy(temp_str, buffer);
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");	
	k = atoi(temp);

	// Initializing disease
	temp = strtok(NULL, " ");	
	disease = malloc(strlen(temp) + 1);
	strcpy(disease, temp);

	temp = strtok(NULL, " ");
	// Checking for date parameters
	if (temp != NULL) 
	{
		date1 = malloc(strlen(temp) + 1);
		strcpy(date1, temp);

		temp = strtok(NULL, " ");
		// Checking for date2 parameter
		if (temp == NULL)
		{
			free(date1);
			free(disease);
			date1 = NULL;
			printf("You didn't give date2 parameter\n");
			return ;
		}
		else
		{
			date2 = malloc(strlen(temp) + 1);
			strcpy(date2, temp);
		}
	}

	list = new_listCases();
	heap = new_tree();

	// Traversing buckets and initializing listCases 
	// each node of listCases has a key (diseaseID) and num of cases  
	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
	{
		if (diseaseHashtable[i] != NULL)
		{
			tempBucket = diseaseHashtable[i];
	
			while (tempBucket != NULL)
			{
				for (int j = 0; j < tempBucket->numOfEntries; ++j)
				{	
					memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
					
					if (strcmp(tempBucketEntry.key, disease) == 0)					
						initialize_listCases(list, tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, "country");
				}
			
				tempBucket = tempBucket->next;
			}
		}
	}		

	initialize_heap(heap, list);

	int numOfNodes = heap->numOfNodes;
	if (k > numOfNodes)
		k = numOfNodes;

	for (int i = 0; i < k; ++i)
		pop(heap, list);

	// Delete remaining heap
	if (k < numOfNodes)
		delete_heap(list, heap, heap->root);

	free(date1);
	free(date2);
	free(disease);
	free(list);
	free(heap);	
}


void insertPatientRecord(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries,
						struct bucket ** countryHashtable, int countryHashtableNumOfEntries, struct list * list, int bucketSize)
{
	count = 0;
	
    patientRecord *tempPatient;
	struct lnode *tempLnode;
	int recordID; 
	char * patientFirstName, * patientLastName, * diseaseID, * country, * entryDate, * exitDate;
	char temp_str[100];
	char * temp;

	strcpy(temp_str, buffer);
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");	

	
	// Initialize recordID	
	recordID = atoi(temp);	
	// Check if recordID already exists
	if (find_patient(list, recordID) != NULL)
	{
		printf("recordID %d already exist\n", recordID);
		return;
	}

	// Initialize patientFirstName
	temp = strtok(NULL, " ");	
	patientFirstName = malloc(strlen(temp) + 1);
	strcpy(patientFirstName, temp);

	// Initialize patientLastName
	temp = strtok(NULL, " ");	
	patientLastName = malloc(strlen(temp) + 1);
	strcpy(patientLastName, temp);

	// Initialize diseaseID
	temp = strtok(NULL, " ");	
	diseaseID = malloc(strlen(temp) + 1);
	strcpy(diseaseID, temp);

	// Initialize country
	temp = strtok(NULL, " ");	
	country = malloc(strlen(temp) + 1);
	strcpy(country, temp);

	// Initialize entryDate
	temp = strtok(NULL, " ");	
	entryDate = malloc(strlen(temp) + 1);
	strcpy(entryDate, temp);

	// Initialize exitDate
	temp = strtok(NULL, " ");	
	// No exitDate given
	if (temp == NULL)
	{
		exitDate = malloc(2);
		exitDate[0] = '-';
		exitDate[1] = '\0';
	}
	else
	{
		exitDate = malloc(strlen(temp) + 1);
		strcpy(exitDate, temp);
		if (compareDates(entryDate, exitDate) > 0)
		{
			printf("Not valid exitDate\n");
			free(patientFirstName);
			free(patientLastName);
			free(diseaseID);
			free(country);
			free(entryDate);
			free(exitDate);

			return;
		}		
	}


	tempPatient = initializePatientRecord(recordID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate);
	tempLnode = new_lnode(tempPatient);
	insert_lnode(list, tempLnode);

	// Insert to diseaseHashtable 

	int hashval = hash(tempPatient->diseaseID, diseaseHashtableNumOfEntries);
	
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

	printf("Record added\n");

	free(patientFirstName);
	free(patientLastName);
	free(diseaseID);
	free(country);
	free(entryDate);
	free(exitDate);

}

void recordPatientExit(char * buffer, struct list * list)
{
	char * exitDate;
	int recordID;
	char temp_str[100];
	char * temp;
	patientRecord * patient;

	// Initialization of recordID, exitDate
	strcpy(temp_str, buffer);
	temp = strtok(temp_str, " ");	
	temp = strtok(NULL, " ");
	recordID = atoi(temp);
	temp = strtok(NULL, " ");
	exitDate = malloc(strlen(temp) + 1);
	strcpy(exitDate, temp);

	patient = find_patient(list, recordID);
	if (patient != NULL) 
	{
		if (compareDates(patient->entryDate, exitDate) > 0)
			printf("Not valid exitDate\n");
		else
		{	
			free(patient->exitDate);
			patient->exitDate = malloc(strlen(exitDate) + 1);
			strcpy(patient->exitDate, exitDate);
			printf("Record updated\n");
		}
	}

	free(exitDate);

}

void numCurrentPatients(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{
	count = 0;
	
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	
	char * virusName;

	char temp_str[100];
	strcpy(temp_str, buffer);
	
	char *temp = strtok(temp_str, " ");
	temp = strtok(NULL, " ");

	// 1st case: /numCurrentPatients
	if (temp == NULL)
	{
		for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		{
			if (diseaseHashtable[i] != NULL)
			{
				tempBucket = diseaseHashtable[i];
				
				while (tempBucket != NULL)
				{
					for (int j = 0; j < tempBucket->numOfEntries; ++j)
					{	
						memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
						printf("%s %d\n", tempBucketEntry.key, 
									get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, NULL, NULL, NULL, 1));
						count = 0;
					}

					tempBucket = tempBucket->next;
				}
			}
		}		
	}
	else
	{
		virusName = malloc(strlen(temp) + 1);
		strcpy(virusName, temp);

		for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
		{
			if (diseaseHashtable[i] != NULL)
			{
				tempBucket = diseaseHashtable[i];
				
				while (tempBucket != NULL)
				{

					for (int j = 0; j < tempBucket->numOfEntries; ++j)
					{	
						memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
						if (strcmp(tempBucketEntry.key, virusName) == 0)
						{
							printf("%s %d\n", tempBucketEntry.key, 
									get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, NULL, NULL, NULL, 1));
							count = 0;
						}
					}

					tempBucket = tempBucket->next;
				}
			}
		}		

		free(virusName);
	}
}