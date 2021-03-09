#include "helpFunc.h"

int bucketSize = 100;
int diseaseHashtableNumOfEntries = 10;

///////////////////////// Worker global signal flags /////////////////////////

volatile sig_atomic_t worker_sigint_sigquit = 0;
volatile sig_atomic_t worker_sigusr1 = 0;

///////////////////////// Parent global signal flags /////////////////////////

volatile sig_atomic_t parent_sigint_sigquit = 0;
volatile sig_atomic_t parent_sigchld = 0;
volatile sig_atomic_t parent_sigusr1 = 0;


//////////////////////////////////////////////////////////////////////////////
DIR * sudo_opendir(char *name)
{
    DIR *dir = opendir(name);
    
    // opendir returns NULL if couldn't open directory
    if (dir == NULL)  
    { 
        perror("Unable to read directory.."); 
        exit(1); 
    }    

    return dir;
}

// Sorts by ascending order date files in directory and returns them in an array
char * sort_date_files(char *name, int *num)
{
    char *dates; // Array with date files 
    char temp_date[11];
    char flag = 0; // Helps us in placing dates in the correct position of the array
    int current_counter = 0;; // Current counter of date files in dates array
    struct dirent *de; // Pointer for directory entry
    DIR *dir;
    int count_date_files = 0; // Number of date files in directory
    int temp = 0; // Temp value

    dir = sudo_opendir(name);

    // Counting date files
    while ((de = readdir(dir)) != NULL)
    { 
        // Ignore . and .. folder
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        ++count_date_files;    
    }      

    closedir(dir);

    // Initializing date array
    // 10 bytes for dd-mm-yyyy and 1 for terminating '\0' 
    dates = malloc(count_date_files * 11); 
    memset(dates, 0, count_date_files * 11);

    dir = sudo_opendir(name);

    while ((de = readdir(dir)) != NULL)
    { 
        flag = 0;

        // Ignore . and .. folder
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        // When dates array is empty place first date file at start 
        if (current_counter == 0)
        {
            memmove(dates, de->d_name, 11);
            ++current_counter;
            continue;
        }

        for (int j = 0; j < current_counter; ++j)
        {
            //  Initializing temp_date
            memmove(temp_date, dates + j*11, 11);
            
            if (compareDates(de->d_name, temp_date) < 0)
            {
                // Moving elements by 1 index in the rray
                // Making space for new date file in the  array  
                memmove(dates + (j+1)*11, dates + j*11, (current_counter-j)*11);
                memmove(dates + j*11, de->d_name, 11);
                flag = 1;
                break;
            }
        }

        // de->d_name is greater than all current date files in array
        // Place it at the end
        if (flag == 0) 
            memmove(dates + current_counter*11, de->d_name, 11);

        ++current_counter;    
    }   
            
    *num = count_date_files;
    closedir(dir);

    return dates;
}

unsigned int hash(char *str, unsigned int HASHSIZE)	// hash function by Dan Bernstein 
														// inspired by http://www.cs.yorku.ca/~oz/hash.html
{
    unsigned int hashval;

    for (hashval = 0; *(str) != '\0'; str++)
        hashval = *str + 31*hashval;
 
    return hashval % HASHSIZE;
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


int get_case_num(char * buffer)
{
	char temp_str[100];
	strcpy(temp_str, buffer);
	char *temp = strtok(temp_str, " ");

	if (strcmp(temp, "/listCountries") == 0)
		return 1;
	else if (strcmp(temp, "/diseaseFrequency") == 0)
		return 2;
	else if (strcmp(temp, "/topk-AgeRanges") == 0)
		return 3;
	else if (strcmp(temp, "/searchPatientRecord") == 0)
		return 4;
	else if (strcmp(temp, "/numPatientAdmissions") == 0)
		return 5;
	else if (strcmp(temp, "/numPatientDischarges") == 0)
		return 6;
	else if (strcmp(temp, "/exit") == 0)
		return 7;
	else 
		return -1;
}

void my_strcat(char ** prev_str, char * add_str)
{
	int prev_str_size = 0, add_str_size = 0, new_size = 0;
	if (*prev_str == NULL)
	{
		* prev_str = malloc(strlen(add_str) + 1);
		memmove(*prev_str, add_str, strlen(add_str) + 1);
	}
	else
	{
		prev_str_size = strlen(*prev_str);
		add_str_size = strlen(add_str);
		new_size = prev_str_size + add_str_size + 1;
		*prev_str = realloc(*prev_str, new_size);
		memmove(*prev_str + prev_str_size, add_str, add_str_size + 1);
	}

	return;
	
}

int count_digits(int num)
{
	int count = 0;

	if (num == 0)
		return 1;

	while (num != 0) 
	{
        num /= 10;         // // Parent code
    // while ((wait(&status)) > 0); // Wait for all children to finish
        ++count;
    }

	return count;
}

void read_message(char ** msgbuf, int bytes_to_read, int bufferSize, int fd)
{
	int current_bytes = 0, temp_bytes = 0, nread = 0;
	char * temp_buffer;

	temp_buffer = malloc(sizeof(char) * bufferSize);
	*msgbuf = malloc(sizeof(char) * bytes_to_read);

    while(current_bytes < bytes_to_read)
    {   
        if (bytes_to_read > bufferSize)
            temp_bytes = bufferSize;
        else 
            temp_bytes = bytes_to_read;    

        if ( (nread = read(fd, temp_buffer, temp_bytes)) < 0 )
        {
            perror ("problem in reading");
            exit(5);
        }

        memmove(*msgbuf+current_bytes,temp_buffer, nread);
        current_bytes += nread;
    }	

	free(temp_buffer);
}

void read_message_with_select(int numWorkers, int fd[numWorkers][2], int bufferSize, char ** message)
{
	int retval, break_flag, max_fd = 0, totalBytes;
	int flag[numWorkers]; // Flags to know if parent read from workers all bytes
	char fifo[100];
	char * msgbuf;
	fd_set rfds;
    
	
    for (int i = 0; i < numWorkers; ++i)
    {
        if (fd[i][1] == -2)
            flag[i] = -1;
        else    
            flag[i] = 0;
    }

    // Reading statistics from workers
    while(1)
    {
        break_flag = 1;

        for (int i = 0; i < numWorkers; ++i)
        {
            if (flag[i] == 0)
            {
                break_flag = 0;
                break;
            }
        }

        if (break_flag == 1)
            break;

        FD_ZERO(&rfds);

        for (int i = 0; i < numWorkers; ++i)
        {
            if (flag[i] == -1)
                continue;

            sprintf(fifo, "worker-%d-out", i);  

            // Opening pipes for reading messages by workers
            if (fd[i][0] == -1)
            {
                if ((fd[i][0] = open(fifo , O_RDWR)) < 0) 
                {
                    perror ("fifo open problem");
                    exit (3);
                }        
            }    

            sudo_fd_set(fd[i][0], &rfds, &max_fd);
        }        

        retval = select(max_fd+1, &rfds, NULL, NULL, NULL);

        // Error in select
        if (retval < 0)
        {
            perror("select");
            exit(3);
        }
        // Data found 
        else if (retval)
        {
            for (int i = 0; i < numWorkers; ++i)
            {
                if (fd[i][0] == -1)
                    continue;

                if (FD_ISSET(fd[i][0], &rfds)) 
                {
                    totalBytes = read_number_of_bytes_of_next_message(bufferSize, fd[i], NULL, NULL, NULL);
                    read_message(&msgbuf, totalBytes, bufferSize, fd[i][0]);
                    my_strcat(message, msgbuf);  
                    free(msgbuf);  
                    flag[i] = 1;    
                }
            }

        }
        // No data found
        else
            printf("There is no data\n");
    }	
	
}

int read_number_of_bytes_of_next_message(int bufferSize, int fd[2], 
                    struct subdirectory_list * subdirectory_list, int * success, int * fail)
{
	int bytes_to_read = sizeof(int), current_bytes = 0 , nread = 0, ret_val, temp_bytes;
    int retval, max_fd = 0;
    fd_set rfds;
    void * msgbuf;
	char * temp_buffer;

	msgbuf = malloc(sizeof(int));
	temp_buffer = malloc(sizeof(char) * bufferSize);

    FD_ZERO(&rfds);
    sudo_fd_set(fd[0], &rfds, &max_fd);

    while (1)
    {
        retval = select(max_fd+1, &rfds, NULL, NULL, NULL);

        if (retval > 0)
            break;
        else
        {
            // Select was interrupted by signal
            if (errno == EINTR)
            {
                // SIGINT or SIGQUIT signal was caught
                if (worker_sigint_sigquit == 1)
                {
                    create_log_file_worker(subdirectory_list, success, fail);
                    worker_sigint_sigquit = 0; // Reset flag
                }
                else if (worker_sigusr1 == 1)
                {
                    function_sigusr1(subdirectory_list, bufferSize, fd[1]);
                    worker_sigusr1 = 0; // Reset flag
                }
                
            }
        }    
    } 

	// Reading number of bytes of upcoming message
	while(current_bytes < bytes_to_read)
	{   
	    if (bytes_to_read > bufferSize)
	        temp_bytes = bufferSize;
	    else 
	        temp_bytes = bytes_to_read;    

	    if ( (nread = read(fd[0], temp_buffer, temp_bytes)) < 0 )
	    {
	        perror ("problem in reading");
	        exit(5);
	    }

	    memmove(msgbuf+current_bytes, temp_buffer, nread);
	    current_bytes += nread;
	}

	// Store number of bytes of upcoming message to retVal
	memmove(&ret_val, msgbuf, sizeof(int));
	// Free memory allocated
	free(msgbuf);	
	free(temp_buffer);

	return ret_val;	
}

void write_message(char * message, int bytes_to_write, int bufferSize, int fd)
{
	int temp_bytes, nwrite;

	while (bytes_to_write > 0)
	{
		if (bytes_to_write > bufferSize)
			temp_bytes = bufferSize;
		else
			temp_bytes = bytes_to_write;

		if ( (nwrite = write(fd, message, temp_bytes)) == -1)
		{ 
			perror ("Error in Writing"); 
			exit(2);
		}

		bytes_to_write -= nwrite; 
		message += nwrite;
	}	
}

void write_number_of_bytes(int bytes, int bufferSize, int fd)
{
    int bytes_to_write = sizeof(int), bytes_of_next_message =  bytes, temp_bytes, nwrite;
    void * temp_str = &bytes_of_next_message; 

    while (bytes_to_write > 0)
    {
        if (bytes_to_write > bufferSize)
            temp_bytes = bufferSize;
        else
            temp_bytes = bytes_to_write;

        if ( (nwrite = write(fd, temp_str, temp_bytes)) == -1)
        { 
            perror ("Error in Writing"); 
            exit(2);
        }

        bytes_to_write -= nwrite; 
        temp_str += nwrite;
    }        	
}

void sudo_fd_set(int fd, fd_set * fds, int * max_fd)
{
	if (max_fd == NULL)
	{
		printf("Error: max_fd is NULL\n");
		return;
	}

    FD_SET(fd, fds);

    if (fd > *max_fd) 
        *max_fd = fd;
}


char * get_country_from_path(char * path)
{
    char * temp_str = path;

    while (1)
    {
        if (*temp_str == '/')
            return temp_str+1;
        else
            temp_str += 1;
    }
}

int country_which_worker(char ** countries_path, char * country, int numWorkers, int numCountries)
{
    int country_exists = -1;
    int num, count = 0;
    char * temp_country = NULL;

    for (int i = 0; i < numCountries; ++i)
    {
        temp_country = get_country_from_path(countries_path[i]);
        
        if (strcmp(temp_country, country) == 0)
        {
            country_exists = 1;
            num = i; 
            break;   
        }
    }

    // Country doesn't exists
    if (country_exists == -1)
        return -1;
    else
    {
        while (1)
        {
            for (int workerID = 0; workerID < numWorkers; ++workerID)
            {
                if (count == num)
                    return workerID;
                
                count++;
            }
        }

    }
}

void create_log_file_worker(struct subdirectory_list * subdirectory_list, int * success, int * fail)
{
    struct subdirectory *temp_subdir = subdirectory_list->start;
    char *name;
	FILE* fp;

    name = malloc(strlen("log_file.") + count_digits(getpid()) + 1);
    sprintf(name, "log_file.%d", getpid());
    fp = fopen(name, "w+");

    while(temp_subdir != NULL)
    {
        fprintf(fp, "%s\n", get_country_from_path(temp_subdir->path));
        temp_subdir = temp_subdir->next;
    }

    fprintf(fp, "TOTAL %d\n", *success + *fail);
    fprintf(fp, "SUCCESS %d\n", *success);
    fprintf(fp, "FAIL %d\n", *fail);
    
    free(name);
    fclose(fp);    
}

void function_sigusr1(struct subdirectory_list * subdirectory_list, int bufferSize, int fd)
{

    int dates_count = 0, numOfPatients = 0;
    struct subdirectory *temp_subdir = subdirectory_list->start;
    char * dates, * date_path, *new_string = NULL, *string, *temp_str, c;
    FILE *fp;

    // Variables used in main for loop
    struct RBTree * tempTree, *addedTree;
    patientRecord *tempPatient = NULL;
	struct lnode *tempLnode;
    struct node *tempTreeNode;	
    int bufferRecordID, bufferAge, hashval, error;
	char * bufferPatientFirstName,  * bufferPatientLastName, * bufferDiseaseID, 
			* bufferCountry, * bufferDate, * bufferEntryDate, * bufferExitDate, * bufferStatus;

    // Allocating space for temp buffers
    bufferPatientFirstName = malloc(sizeof(char) * 100);
    bufferPatientLastName = malloc(sizeof(char) * 100);
    bufferDiseaseID = malloc(sizeof(char) * 100);
    bufferStatus = malloc(sizeof(char) * 6);

    // Traverse through subdirectories
    while (temp_subdir != NULL)
    {   
        date_path = malloc(strlen(temp_subdir->path) + 12);
        dates = sort_date_files(temp_subdir->path, &dates_count);
        temp_str = get_country_from_path(temp_subdir->path);
        addedTree = new_RBTree();

        for (int i = 0; i < dates_count; ++i)
        {   
            // Findd new date_file added
            if (key_exist(temp_subdir->date_files, dates+i*11) == 0)
            {
                insert_date_file(temp_subdir, dates+i*11);
                strcpy(date_path, temp_subdir->path);
                strcat(date_path, "/");
                numOfPatients = 0;
                strcat(date_path, dates + i*11);

                // Opening date file to count number of patients
                if ((fp = fopen (date_path, "r")) == NULL)
                {   
                    perror("opening date files");
                    exit(3);
                }
                while (1)
                {
                    c = fgetc(fp);
            
                    if (feof(fp)) 
                        break ;	

                    if (c=='\n')
                        numOfPatients++;   
                }
                fclose(fp);       

                // Opening date file to initialize our structs
                if ((fp = fopen (date_path, "r")) == NULL)
                {
                    perror("opening date files");
                    exit(3);
                }                   

                // Creating temp tree for every date file
                tempTree = new_RBTree();  

                // Traversing through our patients of each date file
                for (int j = 0; j < numOfPatients; ++j)
                {

                    fscanf(fp, "%d %s %s %s %s %d", &bufferRecordID, bufferStatus, bufferPatientFirstName, 
                                                        bufferPatientLastName, bufferDiseaseID, &bufferAge);

                    tempPatient = RBTgetPatient(temp_subdir->tree , &bufferRecordID, 1);

                    // Record already exists so complete entry or exit date
                    if (tempPatient != NULL) 
                    {
                        // Same id
                        if (tempPatient->entryDate != NULL && strcmp(bufferStatus, "ENTER") == 0)
                        {
                            // printf("Error: id %d already exists\n", bufferRecordID);
                            continue;
                        }        

                        if (strcmp(bufferStatus, "ENTER") == 0)
                        {
                            tempPatient->entryDate = malloc(sizeof(char) * 11);
                            strcpy(tempPatient->entryDate, dates + i*11);
                        } 
                        else // Status is EXIT 
                        {   
                            free(tempPatient->exitDate);
                            tempPatient->exitDate = malloc(sizeof(char) * 11);
                            strcpy(tempPatient->exitDate, dates + i*11);
                        }                                
                    }
                    else
                    {
                        if (strcmp(bufferStatus, "ENTER") == 0)
                        {
                            tempPatient = initializePatientRecord(bufferRecordID, bufferAge, bufferPatientFirstName, 
                                        bufferPatientLastName, bufferDiseaseID, temp_str, dates + i*11, "-");         
                        }
                        else
                        {
                            tempPatient = initializePatientRecord(bufferRecordID, bufferAge, bufferPatientFirstName, 
                                        bufferPatientLastName, bufferDiseaseID, temp_str, NULL, dates + i*11);
                        }

                        // Insert to tempTree
                        tempTreeNode = new_node(tempPatient, 1);
                        RBTinsert(tempTree, tempTreeNode, 1);
                        tempTreeNode = new_node(tempPatient, 1);
                        RBTinsert(temp_subdir->tree, tempTreeNode, 1);
                        tempTreeNode = new_node(tempPatient, 1);
                        RBTinsert(addedTree, tempTreeNode, 1);

                    }                                 
                }

                // Error checking
                error_checking(tempTree, tempTree->root, 0);
                create_statistics(tempTree, dates + i*11, temp_str, &string);
                my_strcat(&new_string, string);
                free(string);

                // Free memory for temp tree
                destroyRBTree(tempTree, tempTree->root, 0);
                free(tempTree);

                fclose(fp);
            }   
        }

        error_checking(addedTree, addedTree->root, 0);
        error_checking(temp_subdir->tree, temp_subdir->tree->root, 1);
        insert_patients_to_structs(addedTree, addedTree->root, temp_subdir, 
                                bucketSize, diseaseHashtableNumOfEntries);                          

        destroyRBTree(addedTree, addedTree->root, 0);
        free(addedTree);        
        free(date_path);
        free(dates);

        temp_subdir = temp_subdir->next;
    }

    // Writing how many bytes are the statistics
    int total_bytes = strlen(new_string) + 1; 
    write_number_of_bytes(total_bytes, bufferSize, fd);

    // Writing statistics
    write_message(new_string, total_bytes, bufferSize, fd);    

    free(new_string);

    // Deleting space of temp buffers
    free(bufferPatientFirstName);	
    free(bufferPatientLastName);	
    free(bufferDiseaseID);
    free(bufferStatus);
}


void error_checking(struct RBTree * T, struct node * node, int flag_delete)
{
	if (T->NIL == NULL || T->root == T->NIL) 
	{
		return ;
	}

	if (node != T->NIL)
	{  
		error_checking(T, node->left, flag_delete);
		error_checking(T, node->right, flag_delete);

		// Checking for errors
		
        // case 1: entryDate doesn't exist
        if (node->patient->entryDate == NULL)
        {
			// printf("Error in patient: ");
			// printRecord(node->patient);
			RBTdeleteKey(T, &node->patient->recordID, 1, flag_delete);            
        }

        // case 2: entryDate >= exitDate
		else if (strcmp(node->patient->exitDate, "-") != 0 && 
			compareDates(node->patient->entryDate, node->patient->exitDate) > 0)
		{
			// printf("Error in patient: ");
			// printRecord(node->patient);
			RBTdeleteKey(T, &node->patient->recordID, 1, flag_delete);
		}
    }
}


void insert_to_tree(struct subdirectory * subdirectory, struct RBTree * T, struct node * node)
{
    struct node * tempTreeNode;

	if (T->NIL == NULL || T->root == T->NIL) 
		return ;

	if (node != T->NIL)
	{  
		insert_to_tree(subdirectory, T, node->left);
		insert_to_tree(subdirectory, T, node->right);

        tempTreeNode = new_node(node->patient, 1);
        RBTinsert(subdirectory->tree, tempTreeNode, 1);
    }    
}

void insert_patients_to_structs(struct RBTree * T, struct node * node, struct subdirectory * subdirectory,
                                int bucketSize, int diseaseHashtableNumOfEntries)
{

    int hashval = -1;

	if (T->NIL == NULL || T->root == T->NIL) 
		return ;

	if (node != T->NIL)
	{  
		insert_patients_to_structs(T, node->left, subdirectory, bucketSize, diseaseHashtableNumOfEntries);
		insert_patients_to_structs(T, node->right, subdirectory, bucketSize, diseaseHashtableNumOfEntries);

        hashval = hash(node->patient->diseaseID, diseaseHashtableNumOfEntries);

        // No buckets in current position of diseaseHashtable
        if (subdirectory->diseaseHashtable[hashval] == NULL)
            subdirectory->diseaseHashtable[hashval] = new_bucket(bucketSize);

        insert_patient_to_bucket(subdirectory->diseaseHashtable[hashval], node->patient, bucketSize);  

	}
}

void create_statistics(struct RBTree * T, char * date, char * country, char ** retString)
{
	struct listCases * listCases;
    struct listNode * temp;
    char temp_string[100];
    int bytes = 0;

    
    listCases = new_listCases();
    initialize_listCases(listCases, T, T->root, NULL, NULL, "diseaseID");
    temp = listCases->start;
    bytes = strlen(date) + strlen("\n") + strlen(country) + strlen("\n"); 


    while (temp != NULL)
    {
        bytes += strlen(temp->key) + strlen("\n");
        bytes += strlen("Age range 0-20 years:  cases\n") + count_digits(temp->age_cases[0]);
        bytes += strlen("Age range 21-40 years:  cases\n") + count_digits(temp->age_cases[1]);
        bytes += strlen("Age range 41-60 years:  cases\n") + count_digits(temp->age_cases[2]);
        bytes += strlen("Age range 60+ years:  cases\n\n") + count_digits(temp->age_cases[3]);     

        temp = temp->next;
    }

    bytes += 1; // +1 byte for '\0'

    *retString = malloc(sizeof(char) * bytes);
    memset(*retString, '\0', bytes);

    strcpy(*retString, date);
    strcat(*retString, "\n");
    
    strcat(*retString, country);
    strcat(*retString, "\n");

    temp = listCases->start;
    while(temp != NULL)
    {
        strcat(*retString, temp->key);
        strcat(*retString, "\n");

        sprintf(temp_string, "Age range 0-20 years: %d cases\n", temp->age_cases[0]);
        strcat(*retString, temp_string);
        sprintf(temp_string, "Age range 21-40 years: %d cases\n", temp->age_cases[1]);
        strcat(*retString, temp_string);
        sprintf(temp_string, "Age range 41-60 years: %d cases\n", temp->age_cases[2]);
        strcat(*retString, temp_string);
        sprintf(temp_string, "Age range 60+ years: %d cases\n\n", temp->age_cases[3]);
        strcat(*retString, temp_string);

        temp = temp->next;
    }

    delete_listCases(listCases);
    free(listCases);

}
