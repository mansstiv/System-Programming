#include "whoServerFunc.h"


// ./whoServer -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize
int main(int argc, char *argv[])
{
    signal(SIGINT, whoServer_handler_sigint);

    int queryPortNum = -1, statisticsPortNum = -1, numThreads = -1, bufferSize = -1, sock, sock2, newsock, err;
    struct sockaddr_in  server, server2, client;
    struct sockaddr * serverptr = (struct sockaddr *) &server, * clientptr = (struct sockaddr *) &client,
                        * server2ptr = (struct sockaddr *) &server2;
    socklen_t clientlen = sizeof(client);    
    struct thread_whoServer_Params params;

    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull , 0);
    pthread_mutex_init (&mtx_whoSerer_variables, 0);
    pthread_mutex_init (&mtx_buffer , 0);
    pthread_mutex_init (&mtx_whoServer_stdout , 0);

    // Checking arguments and initializing corresponding variables 
    for (int i=0; i<argc; i++) 
    {
        if (strcmp(argv[i], "-q") == 0) 
            queryPortNum = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-s") == 0) 
            statisticsPortNum = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-w") == 0) 
            numThreads = atoi(argv[i+1]);   
        else if (strcmp(argv[i], "-b") == 0) 
            bufferSize = atoi(argv[i+1]);   
    }

    // Checking that parameters are valid
    if (queryPortNum == -1 || statisticsPortNum == -1 || numThreads == -1 || bufferSize == -1)
    {
        printf("COMMAND: ./whoServer -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize\n\nInvalid parameters..\n");
        exit(1);
    }

    // Initialize global structs
    initialize_circular_buffer(&buffer, bufferSize);
    initialize_workers_info(&workers_info);

    // Initialize thread parameters
    initialize_thread_whoServer_Params(bufferSize, &params);

    pthread_t threadsID[numThreads] ;

    // Create threads
    for (int i = 0; i < numThreads; ++i)
    { 
        if (err = pthread_create(&threadsID[i] , NULL, whoServer_thread, (void *)&params))
        {
            perror2 ("pthread_create" , err);
            exit(1) ;
        }
    }
    
    // Create socket for communicating with whoClient
    if ((sock = socket( AF_INET , SOCK_STREAM , 0)) < 0)
        perror_exit("socket");   
    
    // Bind on queryPortNum        
    server.sin_family = AF_INET ; // Internet domain 
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(queryPortNum); // The given port  

    if (bind(sock, serverptr, sizeof(server)) < 0)    
        perror_exit("bind");

    // Listening for connections
    if (listen(sock , 1000) < 0) 
        perror_exit("listen");    
    printf ("Listening for connections to queryPortNum %d\n", queryPortNum);        

    // Create socket for communicating with workers
    if ((sock2 = socket( AF_INET , SOCK_STREAM , 0)) < 0)
        perror_exit("socket");     

    // Bind on statisticsPortNum        
    server2.sin_family = AF_INET ; // Internet domain 
    server2.sin_addr.s_addr = htonl(INADDR_ANY);
    server2.sin_port = htons(statisticsPortNum); // The given port   

    if (bind(sock2, server2ptr, sizeof(server)) < 0)    
        perror_exit("bind");          
    
    // Listening for connections
    if (listen(sock2 , 1000) < 0) 
        perror_exit("listen"); 
    printf ("Listening for connections to statisticsPortNum %d\n", statisticsPortNum);

    struct hostent * rem;
    char clientIP[50];
    fd_set rfds;
    struct timeval timeout;
    int max_fd = 0, retval, count = 0;
    int * workerPorts = NULL;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // First read statistics 
    while (1)
    {
        FD_ZERO(&rfds); 
        sudo_fd_set(sock2, &rfds, &max_fd);
        sudo_fd_set(sock, &rfds, &max_fd);

        retval = select(max_fd+1, &rfds, NULL, NULL, NULL);

        if (retval > 0)
        {
            if (FD_ISSET(sock2, &rfds)) // statisticsPortNum
            {
                if ( (newsock = accept(sock2, clientptr, &clientlen)) < 0) 
                    perror_exit("accept");    

                if (count == 0) // Only one time in loop to initialize ip of workers-server
                {
                    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, 
                            sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) 
                    { 
                        herror ("gethostbyaddr") ; 
                        exit (1) ;
                    }                        

                    get_ip(rem, clientIP);
                    workers_info.ip = clientIP;
                }

                ++count;

                // printf("\n\nproducer: %d, workerIP |%s|\n\n", newsock, workers_info.ip);
                place(&buffer, newsock, bufferSize, "Statistics");  
            }
            else if (FD_ISSET(sock, &rfds))  // queryPortNUm
            {
                if ( (newsock = accept(sock, clientptr, &clientlen)) < 0) 
                    perror_exit("accept");    

                place(&buffer, newsock, bufferSize, "Query");   
            }

            pthread_cond_signal(&cond_nonempty);   
        }  
        else 
        {
            // Select was interrupted by signal
            if (errno == EINTR)
            {

                if (whoServer_sigint == 1)
                {
                    free(workers_info.ports_array.array);
                    free(buffer.data);

                    for (int i = 0; i < numThreads; ++i)
                        if(pthread_cancel(threadsID[i]) != 0)
                            printf("error in pthread_cancel\n");

                    pthread_cond_destroy(&cond_nonempty);
                    pthread_cond_destroy(&cond_nonfull);
                    pthread_mutex_destroy(&mtx_buffer);
                    pthread_mutex_destroy(&mtx_whoServer_stdout);
                    pthread_mutex_destroy(&mtx_whoSerer_variables);
                    return 1;
                }
            }
        }
                      
    }    

}
 