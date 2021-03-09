#include "workerFunc.h"

int main(int argc, char const *argv[])
{
    // Set signals
    signal(SIGINT, workerhandler_sigint_sigquit);
    signal(SIGQUIT, workerhandler_sigint_sigquit);
    signal(SIGUSR1, workerhandler_sigusr1);

    // For blocking signals purpose when entering into sensitive section of code
	sigset_t signal_set; 
	sigemptyset(&signal_set);
	sigaddset(&signal_set, SIGINT);
	sigaddset(&signal_set, SIGQUIT);


    int workerID = atoi(argv[1]);
    int total_bytes = atoi(argv[2]); // Total bytes for reading , writting
    int current_bytes = 0; // Current bytes read
                        // Must sum up to "bytes" to finish reading
    int temp_bytes, tempVal;                    
    int bufferSize = atoi(argv[3]); // Size of pipe                    
    char *msgbuf, *string, *new_string;
    char *temp_buffer;
    char fifo[100];
    int fd[2];
    int nread = 0, nwrite = 0; // Bytes being read and written from read() and write()
    struct subdirectory_list * subdirectory_list;
    struct subdirectory * temp_subdir;
    struct RBTree * tempTree, * tempInvalidTree;

    new_string = NULL;
    subdirectory_list = new_subdirectory_list();
    sprintf(fifo, "worker-%d-in", workerID);

    if ((fd[0] = open(fifo , O_RDWR)) < 0) 
    {
        perror ("fifo open problem");
        exit (3);
    }

    // Reading path of subdirectories
    read_message(&msgbuf, total_bytes, bufferSize, fd[0]);

    char *dates, *str = msgbuf, *date_path, *temp_str;
    char c;
    int num = 0, array_length = 0, temp_fd, numOfPatients = 0;
    FILE *fp;

    // Variables used in main for loop
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

    // For every country open files by ascending order
    while(1)
    {
        temp_subdir = new_subdirectory(str, diseaseHashtableNumOfEntries);
        insert_subdirectory(subdirectory_list, temp_subdir);
        dates = sort_date_files(str, &array_length); 
        date_path = malloc(strlen(str) + 12);
        temp_str = get_country_from_path(str);

        // Traversing through date files       
        for(int i = 0; i < array_length; ++i)
        {   
            insert_date_file(temp_subdir, dates+i*11);
            strcpy(date_path, str);
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
                    // if same id
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
                }                                 
            }

            // Error checking and inserting to main tree of subdirectory
            error_checking(tempTree, tempTree->root, 0);
            create_statistics(tempTree, dates + i*11, temp_str, &string);
            my_strcat(&new_string, string);
            free(string);

            // Free memory for temp tree
            destroyRBTree(tempTree, tempTree->root, 0);
            free(tempTree);

            fclose(fp);
        }

        error_checking(temp_subdir->tree, temp_subdir->tree->root, 1);
        insert_patients_to_structs(temp_subdir->tree, temp_subdir->tree->root, temp_subdir, 
                                bucketSize, diseaseHashtableNumOfEntries);                   

        free(date_path);
        free(dates);

        num += strlen(str) + 1;        
        if (num >= total_bytes)
            break;        
        str = &msgbuf[num]; 
    }

    // Writing statistics to pipe
    sprintf(fifo, "worker-%d-out", workerID);

    if ((fd[1] = open(fifo , O_RDWR)) < 0) 
    {
        perror ("fifo open problem");
        exit (3);
    }

    // Writing how many bytes are the statistics
    total_bytes = strlen(new_string) + 1; 
    write_number_of_bytes(total_bytes, bufferSize, fd[1]);

    // Writing statistics
    write_message(new_string, total_bytes, bufferSize, fd[1]);
    
    // Main loop 
    // Reading commands worker will send and answering back 
    
    free(msgbuf);


    char * answer;
    int case_num;
    int success = 0, fail = 0;
    char temp_command[100];

    while(1)
    {
        answer = NULL;

        // Readind command 
        total_bytes = read_number_of_bytes_of_next_message(bufferSize, fd,  
                                                        subdirectory_list, &success, &fail);
        read_message(&msgbuf, total_bytes, bufferSize, fd[0]);
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

        	case 7:
                free(msgbuf);
                if (answer != NULL)
                    free(answer);
                free(new_string);

                fflush(stdout);

                // Deleting space of temp buffers
                free(bufferPatientFirstName);	
                free(bufferPatientLastName);	
                free(bufferDiseaseID);
                free(bufferStatus);

                delete_subdirectory_list(subdirectory_list, diseaseHashtableNumOfEntries);
                
                free(subdirectory_list);
                
                close(fd[0]);
                close(fd[1]);
        		        		
                return 1;

        }
        
        total_bytes = strlen(answer) + 1;
        write_number_of_bytes(total_bytes, bufferSize, fd[1]); 
        write_message(answer, total_bytes, bufferSize, fd[1]);

        // Dealocate space for temp buffers
        free(msgbuf);
        free(answer);
    }

}