#include "masterFunc.h"


int main(int argc, char *argv[])
{

    signal(SIGINT, parenthandler_sigint_sigquit);
    // signal(SIGQUIT, parenthandler_sigint_sigquit);   
    signal(SIGCHLD, parenthandler_sigchld); 
    // signal(SIGUSR1, parenthandler_sigusr1);

    int numWorkers = -1, bufferSize = -1, temp = 0, serverPort = -1;
    char *input_dir = NULL, *serverIP = NULL;
    struct dirent *de; // Pointer for directory entry 
    DIR *dir; 
    pid_t pid;

    // ./master -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir

    // Checking arguments and initializing corresponding variables 
    for (int i=0; i<argc; i++) 
    {
        if (strcmp(argv[i], "-w") == 0) 
            numWorkers = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-b") == 0) 
            bufferSize = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-p") == 0) 
            serverPort = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-s") == 0) 
        {
            serverIP = malloc(strlen(argv[i+1]) + 1);
            strcpy(serverIP, argv[i+1]);
        }         
        else if (strcmp(argv[i], "-i") == 0)
        {
            input_dir = malloc(strlen(argv[i+1]) + 1);
            strcpy(input_dir, argv[i+1]);
        }
    }		

    // Checking for invalid parameters
    if (numWorkers < 0 || bufferSize < 0 ||input_dir == NULL || serverPort == -1 || serverIP == NULL)
    {
        printf("COMMAND: ./master -w numWorkers -b bufferSize -s serverIP -p serverPort -i input_dir\n\nInvalid parameters..\n");
        return 1;
    }

    dir = sudo_opendir(input_dir);

    // Counting countries directories of input_dir
    int countries_count = 0, max_country_len = 0, country_len = 0;
    while ((de = readdir(dir)) != NULL)
    {   
        // Ignore . and .. folder
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        // Taking max len
        country_len = strlen(de->d_name);
        if (country_len > max_country_len)
            max_country_len = country_len;
    
        ++countries_count;
    }

    closedir(dir);

    char **countries_path; // path of every country
    
    // Initializing countries_path array
    countries_path = malloc(sizeof(char *) * countries_count);
    for (int i = 0; i < countries_count; ++i)
        // + 2 because of the '/' and the end of string '\0'
        countries_path[i] = malloc(max_country_len + strlen(input_dir) + 2);

    dir = sudo_opendir(input_dir);

    // Creating countries path
    while ((de = readdir(dir)) != NULL)
    {
        // Ignore . and .. folder
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        sprintf(countries_path[temp], "%s/%s", input_dir, de->d_name);
        ++temp;
    }

    closedir(dir);

    // Creating 2 named pipes for each worker
    char fifo[100];
    for (int i = 0; i < numWorkers; ++i)
    {
        sprintf(fifo, "worker-%d-in", i);
        if (mkfifo (fifo , 0666) == -1)
        {
            if (errno != EEXIST) 
            {
                perror("receiver : mkfifo");
                exit(6);
            }
        }
        sprintf(fifo, "worker-%d-out", i);
        if (mkfifo (fifo , 0666) == -1)
        {
            if (errno != EEXIST) 
            {
                perror("receiver : mkfifo");
                exit(6);
            }
        }
    }
    
    char ** countries_path_for_workers; // subdirectory path for every worker
    int * worker_bytes; // counting bytes of countries path for every worker
                        // useful to know maximum bytes for malloc in worker_country array
    int index = 0; // index for worker_bytes array                    

    // Initializing worker_bytes array
    worker_bytes = malloc(sizeof(int) * numWorkers);
    for (int i = 0; i < numWorkers; ++i)
        worker_bytes[i] = 0; // 1 for terminating string character '\0'
    
    // Counting bytes of every subdirectory for every worker
    for (int i = 0; i < countries_count; ++i)
    {   
        index = i % numWorkers;
        worker_bytes[index] += strlen(countries_path[i]) + 1;
    }   
    
    // Initializing worker_country array
    countries_path_for_workers = malloc(sizeof(char *) * numWorkers);
    for (int i = 0; i < numWorkers; ++i)
    {
        if (worker_bytes[i] == 0)
            countries_path_for_workers[i] = NULL;
        else
        {
            countries_path_for_workers[i] = malloc(sizeof(char) * worker_bytes[i]);    
            memset(countries_path_for_workers[i], '\0', worker_bytes[i]);
        }
    }

    int temp_index=1;
    char prev, next;
    for (int i = 0; i < countries_count; ++i)
    {   
        index = i % numWorkers;
        temp_index = 1;

        if (strlen(countries_path_for_workers[index]) == 0)
            strcat(&countries_path_for_workers[index][0], countries_path[i]);
        else    
        {
            prev = countries_path[index][0];
            next = countries_path[index][1];
            while(temp_index < worker_bytes[index])
            {   

                if (prev == '\0' && next == '\0')
                    break;
                else
                {
                    ++temp_index;
                    prev = countries_path_for_workers[index][temp_index - 1];
                    next = countries_path_for_workers[index][temp_index];
                }    
            }

            strcat(&countries_path_for_workers[index][temp_index], countries_path[i]);
        }
    }

    int fd[numWorkers][2]; // file descriptor for pipes
    char file_desc[5]; 
    char total_bytes[12]; // Total bytes each worker must read 
    char buffer_size[12]; // Size of pipe
    char workerID[10];
    char *temp_str;
    int nwrite, nread; // Number of bytes written and read
    int totalBytes; // Total bytes parent wants to write to worker
    int temp_bytes;

    sprintf(buffer_size, "%d", bufferSize); 
    for (int i = 0; i < numWorkers; ++i)
    {
        // Mark with -2 workers that don't need to work
        if (countries_path_for_workers[i] == NULL)       
        {
            fd[i][0] = -2;  
            fd[i][1] = -2;
        }
        // Initialize rest of worker's pipes fd's with -1
        else
        {
            fd[i][0] = -1;  
            fd[i][1] = -1;            
        }
             
    }

    // Array with pid for every worker
    // -1  will have extra workers that don't work
    int workers_pids[numWorkers];
    for(int i = 0; i < numWorkers; ++i)
        workers_pids[i] = -1;

    // Creating numWorkers processes
    for (int i = 0; i < numWorkers; ++i) 
    {   
        if (countries_path_for_workers[i] == NULL)
                continue;

        sprintf(fifo, "worker-%d-in", i);

        // Fork process
        if ((pid = fork()) == -1)
        {
            perror (" fork "); 
            exit (1);
        }

        // Parent code
        // Giving subdirectory paths to every worker
        if (pid > 0)
        {
            workers_pids[i] = pid;

            if ((fd[i][1] = open(fifo , O_RDWR)) < 0) 
            {
                perror ("fifo open problem");
                exit (3);
            }
            
            // Writing serverIP
            temp = strlen(serverIP) + 1;
            write_number_of_bytes(temp, bufferSize, fd[i][1]);
            write_message(serverIP, temp, bufferSize, fd[i][1]);

            // Writing serverPort
            write_number_of_bytes(serverPort, bufferSize, fd[i][1]);

            // Writing paths of subdirectories
            write_message(countries_path_for_workers[i], worker_bytes[i], bufferSize, fd[i][1]);
        }    
        // Children code
        else
        {  
            sprintf(workerID,"%d",i);
            sprintf(total_bytes, "%d", worker_bytes[i]);
            char *args[] = {"./worker", workerID, total_bytes, buffer_size, NULL};
            execvp("./worker", args);            
        }
    }

    char * buffer = NULL, ch; // Temp buffer for commands of user
    int case_num, retval, max_fd = 0, success = 0, fail = 0, counter = 0;
    fd_set rfds;    
    FD_ZERO(&rfds);
    sudo_fd_set(1, &rfds, &max_fd);

    // Main loop
    // Reading commands by user and sending to them to workers
    // Reading answers being sent by workers and printing to sdout

    while (1)
    {
		// printf("1. /listCountries\n");
		// printf("2. /diseaseFrequency virusName date1 date2 [country]\n");
		// printf("3. /topk-AgeRanges k country disease date1 date2\n");
		// printf("4. /searchPatientRecord recordID\n");			
		// printf("5. /numPatientAdmissions disease date1 date2 [country]\n");
		// printf("6. /numPatientDischarges disease date1 date2 [country]\n");
		// printf("7. /exit\n\n");
        // printf("Parent pid: %d\n\n", getpid());

		buffer = malloc(sizeof(char) * 1000);

        while (1)
        {
            retval = select(2, &rfds, NULL, NULL, NULL);

            if (retval > 0)
                break;
            else
            {
                // Select was interrupted by signal
                if (errno == EINTR)
                {
                    // SIGINT or SIGQUIT signal was caught
                    if (parent_sigint_sigquit == 1)
                    {
                        exit_with_sigint_sigquit(countries_path, countries_count, numWorkers, workers_pids,
                                                    &success, &fail);  
                        parent_sigint_sigquit = 0;                                                  
                    }
                    else if (parent_sigchld == 1)
                    {
                        sigchld_function(numWorkers, workers_pids, countries_path_for_workers, worker_bytes,
                                            fd, bufferSize);                       
                    
                        parent_sigchld = 0;
                    }
                    else if (parent_sigusr1 == 1)
                    {
                        // You have to read statistics
                        sigurs1_function(numWorkers, fd, bufferSize);
                    }
                    
                }

            }    
        } 

        while(read(1, &ch, 1))
        {
            if (ch == '\n')
            {
                memmove(buffer+counter, "\0", 1);
                break;
            }
            else
            {                      
                memmove(buffer+counter, &ch, 1);
                counter++;
            }

        }

        counter = 0;
        case_num = get_case_num(buffer);

        switch (case_num)
        {
        	case -1:
        		printf("\n");
        		printf("Wrong command, type again!\n");
                fail++;
        		break;
        	
        	case 1:	
       			printf("\n");
                listCountries(buffer, bufferSize, numWorkers,fd);
                success++;
        		break;
        	
        	case 2:	
        		printf("\n");
                diseaseFrequency(buffer, bufferSize, numWorkers, fd, countries_path, countries_count, &success, &fail);
        		break;
                
        	case 3:
        		printf("\n");
                topkAgeRanges(buffer, bufferSize, numWorkers, fd, countries_path, countries_count, &success, &fail);
        		break;

        	case 4:
        		printf("\n");
                searchPatientRecord(buffer, bufferSize, numWorkers, fd);
                success++;
        		break;

        	case 5:
        		printf("\n");
                numPatientAdmissions(buffer, bufferSize, numWorkers, fd, countries_path, countries_count, &success, &fail);
        		break;

        	case 6:
        		printf("\n");
                numPatientDischarges(buffer, bufferSize, numWorkers, fd, countries_path, countries_count, &success, &fail);
        		break;

        	case 7:
        		printf("exiting\n");

                totalBytes = strlen(buffer) + 1;

                for (int i = 0; i < numWorkers; ++i)
                {
                    if (fd[i][1] == -2)
                        break;

                    write_number_of_bytes(totalBytes, bufferSize, fd[i][1]);
                    write_message(buffer, totalBytes, bufferSize, fd[i][1]);		
                }

                // Wait for children processes to finish
                pid_t wpid;
                int status = 0;
                while ((wpid = wait(&status)) > 0);

                // Free memory that was allocated
                for (int i = 0; i < countries_count; ++i)
                    free(countries_path[i]);
                free(countries_path); 

                for (int i = 0; i < numWorkers; ++i)
                {
                    if (fd[i][0] != -1 && fd[i][0] != -2)
                        close(fd[i][0]);
                    if (fd[i][1] != -1 && fd[i][1] != -2)
                        close(fd[i][1]);

                    free(countries_path_for_workers[i]);
                }    

                free(buffer);
                free(countries_path_for_workers);
                free(worker_bytes);        
                free(input_dir);
        		free(serverIP);

        		return 1;

        }

        printf("\n");
        free(buffer);           
    }

}