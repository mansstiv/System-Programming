
#include "workerFunc.h"

int get_port(int sockfd)
{    
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        return ntohs(sin.sin_port);    
}

void worker_server(int newsock, int bufferSize, int fd[2], struct subdirectory_list * subdirectory_list, int diseaseHashtableNumOfEntries
            , char * new_string, char * serverIP, char * bufferPatientFirstName, char * bufferPatientLastName, 
                char * bufferDiseaseID, char * bufferStatus)
{

    char * answer = NULL, * msgbuf;
    int case_num, nread, num;
    int success = 0, fail = 0;
    char temp_command[100]; char temp_string[100];   
    void * total_bytes = malloc(sizeof(int));

    // Read bytes of next message
    if ( (nread = read(newsock, total_bytes, sizeof(int))) < 0 )
    {
        perror ("problem in reading");
        exit(5);
    }

    // Read actual command
    read_message(&msgbuf, *(int *)total_bytes, *(int *)total_bytes, newsock);

    case_num = get_case_num(msgbuf);

    // Decoding command and answering back
    switch (case_num)
    {
        case 1:	
            listCountriesWorker(&answer, subdirectory_list, &success);
            break;
        
        case 2:	
            diseaseFrequencyWorker(msgbuf, subdirectory_list, diseaseHashtableNumOfEntries, &answer, &success, &fail);
            break;
            
        case 3:
            topkAgeRangesWorker(msgbuf, subdirectory_list, diseaseHashtableNumOfEntries, &answer, &success, &fail);
            break;

        case 4:
            strcpy(temp_command, msgbuf);
            char *token = strtok(temp_command, " ");
            // Initialize recordID
            token = strtok(NULL, " ");
            int recordID = atoi(token);
            searchPatientRecordWorker(recordID,  subdirectory_list, &answer, &success, &fail);
            break;

        case 5:
            numPatientAdmissionsWorker(msgbuf, subdirectory_list, diseaseHashtableNumOfEntries, &answer, &success, &fail);
            break;

        case 6:
            numPatientDischargesWorker(msgbuf, subdirectory_list, diseaseHashtableNumOfEntries, &answer, &success, &fail);
            break;

    }
        
    write_number_of_bytes(strlen(answer) + 1, sizeof(int), newsock); 
    write_message(answer, strlen(answer) + 1, strlen(answer) + 1, newsock);

    // Dealocate space for temp buffers
    free(msgbuf);
    free(answer);
    free(total_bytes);
    close(newsock);

    return;

}

void listCountriesWorker (char ** answer, struct subdirectory_list * subdirectory_list, int * success)
{
    int pid = getpid();
    char * country = NULL, * temp_str = NULL;

    struct subdirectory *temp = subdirectory_list->start;
    
    while(temp != NULL)
    {        
        country = get_country_from_path(temp->path);
        // + 1 byte for '\0'
        temp_str = malloc(strlen(country) + strlen(" ") + count_digits(pid) + strlen("\n") + 1);
        sprintf(temp_str, "%s %d\n", country, pid);
        my_strcat(answer, temp_str);
        free(temp_str);
        temp = temp->next;
    }

    (*success)++;
	     
}

void diseaseFrequencyWorker(char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail)
{

    struct subdirectory *temp_subdir = subdirectory_list->start;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	    
    char * virusName;
	char * date1, * date2;
    char * country = NULL;   
    char * temp; 
    char temp_str[100];
    int total_cases = 0;
    char error = 1;

    strcpy(temp_str, command);
	temp = strtok(temp_str, " ");

    // Initialize virusName
	temp = strtok(NULL, " "); 
    virusName = malloc(strlen(temp) + 1);
    strcpy(virusName, temp);

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);    

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);    

    // Points to [country]
    temp = strtok(NULL, " ");

    // No specific country is given
    if (temp == NULL)
    {
        // Traverse through all countries
        while(temp_subdir != NULL)
        {        
            // Traverse through diseaseHashTable
            for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
            {
                if (temp_subdir->diseaseHashtable[i] != NULL)
                {
                    tempBucket = temp_subdir->diseaseHashtable[i];

                    // Traverse through list of buckets
    				while (tempBucket != NULL)
                    {
                        // Scan every bucketEntry and search for virusName
                        for (int j = 0; j < tempBucket->numOfEntries; ++j)
                        {	
                            memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                            if (strcmp(tempBucketEntry.key, virusName) == 0)
                            {
                                error = 0;
                                total_cases += get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 0);
                            
                            }
                        }
                    
                        tempBucket = tempBucket->next;
                    }                
                }
            }

            temp_subdir = temp_subdir->next;
        }
    }
    else
    {
        country = malloc(strlen(temp) + 1);
        strcpy(country, temp);

        // Traverse through all countries and search for given country
        while(temp_subdir != NULL)
        {        
            if (strcmp(get_country_from_path(temp_subdir->path), country) == 0)
            {
                // Traverse through diseaseHashTable
                for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
                {
                    if (temp_subdir->diseaseHashtable[i] != NULL)
                    {
                        tempBucket = temp_subdir->diseaseHashtable[i];

                        // Traverse through list of buckets
                        while (tempBucket != NULL)
                        {
                            // Scan every bucketEntry and search for virusName
                            for (int j = 0; j < tempBucket->numOfEntries; ++j)
                            {	
                                memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                                if (strcmp(tempBucketEntry.key, virusName) == 0)
                                {
                                    error = 0;
                                    total_cases += get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 0);
                                
                                }
                            }
                        
                            tempBucket = tempBucket->next;
                        }                
                    }
                }

                break;
            }
            else
                temp_subdir = temp_subdir->next;
        }        
    }

    // virusName wasn't found
    if (error == 1)
       (*fail)++;
    else        
        (*success)++;

    *answer = malloc(count_digits(total_cases) + strlen("\n") + 1);
    sprintf(*answer, "%d\n", total_cases);

    free(virusName);
	free(date1);
	free(date2);
	if (country != NULL)
		free(country);
}

void topkAgeRangesWorker (char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail)
{
    struct subdirectory *temp_subdir = subdirectory_list->start;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	    
	struct listCases * list;
	struct tree * heap;    
    char * virusName;
	char * date1, * date2;
    char * country = NULL;   
    char * temp; 
    char temp_str[100];
    int total_cases = 0, k = 0;
    char error = 1;

    strcpy(temp_str, command);
	temp = strtok(temp_str, " ");

    // Initialize k
    temp = strtok(NULL, " ");
    k = atoi(temp);

    // Initialize Country
    temp = strtok(NULL, " ");
    country = malloc(strlen(temp) + 1);
    strcpy(country, temp);  

    // Initialize virusName
	temp = strtok(NULL, " "); 
    virusName = malloc(strlen(temp) + 1);
    strcpy(virusName, temp);

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);    

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);      

    list = new_listCases();
    heap = new_tree();

    // Traverse through all countries and search for given country
    while(temp_subdir != NULL)
    {        
        if (strcmp(get_country_from_path(temp_subdir->path), country) == 0)
        {
            // Traverse through diseaseHashTable
            for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
            {
                if (temp_subdir->diseaseHashtable[i] != NULL)
                {
                    tempBucket = temp_subdir->diseaseHashtable[i];

                    // Traverse through list of buckets
                    while (tempBucket != NULL)
                    {
                        // Scan every bucketEntry and search for virusName
                        for (int j = 0; j < tempBucket->numOfEntries; ++j)
                        {	
                            memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                            if (strcmp(tempBucketEntry.key, virusName) == 0)
                            {
                                error = 0;
                                initialize_listCases(list, tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, "Age");
                            }
                        }
                    
                        tempBucket = tempBucket->next;
                    }                
                }
            }

            break;
        }
        else
            temp_subdir = temp_subdir->next;
    }        

    initialize_heap(heap, list);
    
    char * temp_string = NULL;
    int numOfNodes = heap->numOfNodes;
        
    if (k > numOfNodes)
        k = numOfNodes;

    for (int i = 0; i < k; ++i)
    {
        pop(heap, list, &temp_string);     
        my_strcat(answer, temp_string);
        free(temp_string);
    }

	// Delete remaining heap
	if (k < numOfNodes)
		delete_heap(list, heap, heap->root);    

    if (error == 0)
        (*success)++;
    else
        (*fail)++;

    if (*answer == NULL)
    {
        *answer = malloc(3);
        strcpy(*answer, "-\n");
    }
        

	free(date1);
	free(date2);
    free(virusName);
	free(country);
	free(list);
	free(heap);        
}

void searchPatientRecordWorker (int recordID, struct subdirectory_list * subdirectory_list, char ** answer,
                                int * success, int * fail)
{
    struct subdirectory *temp_subdir = subdirectory_list->start;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	  
    patientRecord * tempRec;
    char * token, * tempData;
    char temp_str[100];

    while(temp_subdir != NULL)
    {
        tempRec = RBTgetPatient(temp_subdir->tree, &recordID, 1);
        
        // Patient was found
        if (tempRec != NULL)
        {
            tempData  = malloc(count_digits(tempRec->recordID) + strlen(" ") + strlen(tempRec->patientFirstName) + 
                    + strlen(" ") + strlen(tempRec->patientLastName) + strlen(" ") + strlen(tempRec->diseaseID) + 
                    + strlen(" ") + count_digits(tempRec->age) + strlen(" ") + strlen(tempRec->entryDate) + 
                    strlen(" ") + strlen(tempRec->exitDate) + strlen("\n") + 1);
            
            sprintf(tempData, "%d %s %s %s %d %s %s\n", tempRec->recordID, tempRec->patientFirstName, tempRec->patientLastName,
                        tempRec->diseaseID, tempRec->age, tempRec->entryDate, tempRec->exitDate);        
            
            my_strcat(answer, tempData);    
            free(tempData);    
            temp_subdir = temp_subdir->next;
        }
        else
            temp_subdir = temp_subdir->next;
    }     

    // Record with recordID wasn't found
    if (*answer == NULL)
    {    
        * answer = malloc(strlen("-") + strlen("\n") + 1);
        strcpy(*answer, "-\n");
        (*fail)++;
    }
    else
        (*success)++;       

}

void numPatientAdmissionsWorker(char * command, struct subdirectory_list * subdirectory_list, 
                                int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail)
{
    struct subdirectory *temp_subdir = subdirectory_list->start;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	    
    char * virusName;
	char * date1, * date2;
    char * country = NULL;   
    char * temp, * temp_str2; 
    char temp_str[100];
    int total_cases = 0;

    strcpy(temp_str, command);
	temp = strtok(temp_str, " ");

    // Initialize virusName
	temp = strtok(NULL, " "); 
    virusName = malloc(strlen(temp) + 1);
    strcpy(virusName, temp);

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);    

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);    

    // Points to [country]
    temp = strtok(NULL, " ");

    // No specific country is given
    if (temp == NULL)
    {
        // Traverse through all countries
        while(temp_subdir != NULL)
        {   
            // Traverse through diseaseHashTable
            for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
            {
                if (temp_subdir->diseaseHashtable[i] != NULL)
                {
                    tempBucket = temp_subdir->diseaseHashtable[i];

                    // Traverse through list of buckets
    				while (tempBucket != NULL)
                    {
                        // Scan every bucketEntry and search for virusName
                        for (int j = 0; j < tempBucket->numOfEntries; ++j)
                        {	
                            memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                            if (strcmp(tempBucketEntry.key, virusName) == 0)
                            {
                                total_cases = get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 0);
                                temp_str2 = malloc(strlen(get_country_from_path(temp_subdir->path)) + strlen(" ") + count_digits(total_cases) + strlen("\n") + 1);
                                sprintf(temp_str2, "%s %d\n", get_country_from_path(temp_subdir->path), total_cases);
                                my_strcat(answer, temp_str2);
                                free(temp_str2);
                            }
                        }
                    
                        tempBucket = tempBucket->next;
                    }                
                }
            }

            temp_subdir = temp_subdir->next;
        }
    }
    else
    {
        country = malloc(strlen(temp) + 1);
        strcpy(country, temp);

        // Traverse through all countries and search for given country
        while(temp_subdir != NULL)
        {        
            if (strcmp(get_country_from_path(temp_subdir->path), country) == 0)
            {
                // Traverse through diseaseHashTable
                for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
                {
                    if (temp_subdir->diseaseHashtable[i] != NULL)
                    {
                        tempBucket = temp_subdir->diseaseHashtable[i];

                        // Traverse through list of buckets
                        while (tempBucket != NULL)
                        {
                            // Scan every bucketEntry and search for virusName
                            for (int j = 0; j < tempBucket->numOfEntries; ++j)
                            {	
                                memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                                if (strcmp(tempBucketEntry.key, virusName) == 0)
                                {
                                    total_cases = get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 0);
                                    *answer = malloc(strlen(country) + strlen(" ") + count_digits(total_cases) + strlen("\n") + 1);
                                    sprintf(*answer, "%s %d\n", country, total_cases);
                                }
                            }
                        
                            tempBucket = tempBucket->next;
                        }                
                    }
                }

                break;
            }
            else
                temp_subdir = temp_subdir->next;
        }        
    }

    if (*answer == NULL)
    {
        *answer = malloc(3);
        strcpy(*answer, "-\n");
        (*fail)++;
    }    
    else
        (*success)++;

    free(virusName);
	free(date1);
	free(date2);
	if (country != NULL)
		free(country);
}

void numPatientDischargesWorker (char * command, struct subdirectory_list * subdirectory_list, 
                            int diseaseHashtableNumOfEntries, char ** answer, int * success, int * fail)
{
    struct subdirectory *temp_subdir = subdirectory_list->start;
	struct bucket * tempBucket;
	struct bucketEntry tempBucketEntry;	    
    char * virusName;
	char * date1, * date2;
    char * country = NULL;   
    char * temp, * temp_str2; 
    char temp_str[100];
    int total_cases = 0;

    strcpy(temp_str, command);
	temp = strtok(temp_str, " ");

    // Initialize virusName
	temp = strtok(NULL, " "); 
    virusName = malloc(strlen(temp) + 1);
    strcpy(virusName, temp);

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);    

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);    

    // Points to [country]
    temp = strtok(NULL, " ");

    // No specific country is given
    if (temp == NULL)
    {
        // Traverse through all countries
        while(temp_subdir != NULL)
        {   
            // Traverse through diseaseHashTable
            for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
            {
                if (temp_subdir->diseaseHashtable[i] != NULL)
                {
                    tempBucket = temp_subdir->diseaseHashtable[i];

                    // Traverse through list of buckets
    				while (tempBucket != NULL)
                    {
                        // Scan every bucketEntry and search for virusName
                        for (int j = 0; j < tempBucket->numOfEntries; ++j)
                        {	
                            memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                            if (strcmp(tempBucketEntry.key, virusName) == 0)
                            {
                                total_cases = get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 1);
                                temp_str2 = malloc(strlen(get_country_from_path(temp_subdir->path)) + strlen(" ") + count_digits(total_cases) + strlen("\n") + 1);
                                sprintf(temp_str2, "%s %d\n", get_country_from_path(temp_subdir->path), total_cases);
                                my_strcat(answer, temp_str2);
                                free(temp_str2);
                            }
                        }
                    
                        tempBucket = tempBucket->next;
                    }                
                }
            }

            temp_subdir = temp_subdir->next;
        }
    }
    else
    {
        country = malloc(strlen(temp) + 1);
        strcpy(country, temp);

        // Traverse through all countries and search for given country
        while(temp_subdir != NULL)
        {        
            if (strcmp(get_country_from_path(temp_subdir->path), country) == 0)
            {
                // Traverse through diseaseHashTable
                for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
                {
                    if (temp_subdir->diseaseHashtable[i] != NULL)
                    {
                        tempBucket = temp_subdir->diseaseHashtable[i];

                        // Traverse through list of buckets
                        while (tempBucket != NULL)
                        {
                            // Scan every bucketEntry and search for virusName
                            for (int j = 0; j < tempBucket->numOfEntries; ++j)
                            {	
                                memmove(&tempBucketEntry, tempBucket->bytes + j*sizeof(struct bucketEntry), sizeof(struct bucketEntry));
                                if (strcmp(tempBucketEntry.key, virusName) == 0)
                                {
                                    total_cases = get_number_of_patients(tempBucketEntry.tree, tempBucketEntry.tree->root, date1, date2, 1);
                                    *answer = malloc(strlen(country) + strlen(" ") + count_digits(total_cases) + strlen("\n") + 1);
                                    sprintf(*answer, "%s %d\n", country, total_cases);
                                }
                            }
                        
                            tempBucket = tempBucket->next;
                        }                
                    }
                }

                break;
            }
            else
                temp_subdir = temp_subdir->next;
        }        
    }

    if (*answer == NULL)
    {
        *answer = malloc(3);
        strcpy(*answer, "-\n");
        (*fail)++;
    }    
    else
        (*success)++;

    free(virusName);
	free(date1);
	free(date2);
	if (country != NULL)
		free(country);    
}                            

/////////////////////////////////////////

void workerhandler_sigint_sigquit(int signo)
{
    worker_sigint_sigquit = 1; // set flag
}

void workerhandler_sigusr1(int signo)
{
    worker_sigusr1 = 1; // set flag
}




