#include "whoClientFunc.h"


// ./whoClient -q queryFile -w numThreads -sp servPort -sip servIP
int main(int argc, char *argv[])
{
    char * queryFile = NULL, * serverIP = NULL;
    int numThreads = -1, serverPort = -1, sock = -1, tempVal;

    // Checking arguments and initializing corresponding variables 
    for (int i=0; i<argc; i++) 
    {
        if (strcmp(argv[i], "-q") == 0) 
        {
            queryFile = malloc(strlen(argv[i+1]) + 1);
            strcpy(queryFile, argv[i+1]);
        }
        else if (strcmp(argv[i], "-w") == 0) 
            numThreads = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-sp") == 0) 
            serverPort = atoi(argv[i+1]);   
        else if (strcmp(argv[i], "-sip") == 0) 
        {
            serverIP = malloc(strlen(argv[i+1]) + 1);
            strcpy(serverIP, argv[i+1]);
        }         
    }	    

    // Checking that parameters are valid
    if (queryFile == NULL || numThreads == -1 || serverPort == -1 || serverIP == NULL)
    {
        printf("COMMAND: ./whoClient -q queryFile -w numThreads -sp servPort -sip servIP\n\nInvalid parameters..\n");
        exit(1);
    }

    // Reading queryFile

    FILE *fp;
    int numOfCommands = 0;
    char c;

    if ((fp = fopen (queryFile, "r")) == NULL)
    {   
        perror("opening query file");
        exit(3);
    }
    while (1)
    {
        c = fgetc(fp);

        if (feof(fp)) 
            break ;	

        if (c=='\n')
            numOfCommands++;
    }
    fclose(fp);


    // No need to create extra threads
    if (numOfCommands < numThreads)
        numThreads = numOfCommands; 

    // Initializing commands array

    size_t len = 0;
    char * line = NULL, ** commands;
    ssize_t nread = 0;

    commands = malloc(sizeof(char *) * numOfCommands);

    if ((fp = fopen (queryFile, "r")) == NULL)
    {   
        perror("opening query file");
        exit(3);
    }          
    
    tempVal = 0;
    while((nread = getline(&line, &len, fp)) != -1)
    {
        commands[tempVal] = malloc(sizeof(char) * strlen(line));
        memmove(commands[tempVal], line, strlen(line));
        commands[tempVal][strlen(line)-1] = '\0'; // replace '\n' with '\0'
        tempVal++;
    }    

    free(line);
    fclose(fp);
    
    // Initializing commands_bytes_for_every_thread array

    int * commands_bytes_for_every_thread;
    
    commands_bytes_for_every_thread = malloc(numThreads * sizeof(int));
    for(int i = 0; i < numThreads; ++i)
        commands_bytes_for_every_thread[i] = 0;

    int index = 0;

    for (int i = 0; i < numOfCommands; ++i)
    {
        index = i % numThreads;
        commands_bytes_for_every_thread[index] += strlen(commands[i]) + 1;
    }

    // Initializing array that has commands for every thread
    char ** commands_for_every_thread;
    int temp_index=1;
    char prev, next;
    
    commands_for_every_thread = malloc(sizeof(char *) * numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
        commands_for_every_thread[i] = malloc(sizeof(char) * commands_bytes_for_every_thread[i]);   
        memset(commands_for_every_thread[i], '\0', commands_bytes_for_every_thread[i]);
    }

    if ((fp = fopen (queryFile, "r")) == NULL)
    {   
        perror("opening query file");
        exit(3);
    }          

    for (int i = 0; i < numOfCommands; ++i)
    {   
        index = i % numThreads;
        temp_index = 1;

        if (strlen(commands_for_every_thread[index]) == 0)
            strcat(&commands_for_every_thread[index][0], commands[i]);
        else    
        {
            prev = commands[index][0];
            next = commands[index][1];
            while(temp_index < commands_bytes_for_every_thread[index])
            {   

                if (prev == '\0' && next == '\0')
                    break;
                else
                {
                    ++temp_index;
                    prev = commands_for_every_thread[index][temp_index - 1];
                    next = commands_for_every_thread[index][temp_index];
                }    
            }

            strcat(&commands_for_every_thread[index][temp_index], commands[i]);
        }
    }       
    fclose(fp);

    
    // Initialize array of threads ids
    int err;
    pthread_t * threadsID ;
    struct thread_whoClient_Params params[numThreads];

    pthread_cond_init(&cv, NULL); // Initialize condition variable

    // Initialize parameters for thread function
    for (int i = 0; i < numThreads; ++i)
        initialize_thread_whoClient_Params(serverIP, serverPort, commands_for_every_thread[i], numThreads, 
                                            commands_bytes_for_every_thread[i], &params[i]);

    if ((threadsID = malloc(numThreads * sizeof(pthread_t))) == NULL) 
    {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < numThreads; ++i)
    {
        // Create a thread 
        if (err = pthread_create(threadsID + i , NULL, thread_whoClient, (void *)&params[i])) 
        {
            perror2 ("pthread_create" , err);
            exit(1) ;
        }
    }


    // Wait for all threads to finish
    for (int i =0 ; i < numThreads; ++i)
    {
        if (err = pthread_join(threadsID[i], NULL)) 
        {        
            perror2("pthread_join", err); 
            exit(1);
        }
    }

    printf("main: all threads terminated\n");

    // Destroy mutexes, condition variables, and deallocate space for all variables

    if (err = pthread_mutex_destroy(&mtx)) 
    {
        perror2 ("pthread_mutex_destroy" , err); 
        exit(1); 
    }
    if (err = pthread_mutex_destroy(&mtx_stdout)) 
    {
        perror2 ("pthread_mutex_destroy" , err); 
        exit(1); 
    }    
    if (err = pthread_cond_destroy(&cv)) 
    {
        perror2 ("pthread_cond_destroy" , err); 
        exit(1); 
    }

    free(threadsID);
    free(queryFile);
    free(serverIP);
    free(commands_bytes_for_every_thread);

    for (int i = 0; i < numThreads; ++i)
        free(commands_for_every_thread[i]);
    free(commands_for_every_thread);    

    for (int i = 0; i < numOfCommands; ++i)
        free(commands[i]);
    free(commands);    

    pthread_exit(NULL);
}

