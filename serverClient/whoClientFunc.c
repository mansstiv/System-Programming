# include "whoClientFunc.h"

int count_threads = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // Mutex for condition variable
pthread_mutex_t mtx_stdout = PTHREAD_MUTEX_INITIALIZER; // Mutex for printing to stdout
pthread_cond_t cv; // Condition variable

void * thread_whoClient(void * params)
{
    struct thread_whoClient_Params * argumments = params;
    int err, sock, nread;
    struct sockaddr_in server;
    struct sockaddr * serverptr = (struct sockaddr *) &server;
    void * total_bytes = malloc(sizeof(int));
    char * temp_str;
    struct hostent * rem ;


    // Lock mutex 
    if (err = pthread_mutex_lock(&mtx))
    {
        perror2("pthread_mutex_lock", err); 
        exit(1); 
    }
    count_threads++;
    // printf("count %d\n", count_threads);

    // Last thread so wake all others and continue
    if (count_threads == argumments->numThreads)
    {
        pthread_cond_broadcast(&cv);
        // printf("count_threads = %d, ksypnaw ta threads\n", count_threads);
    }

    // Wait for all threads
    while (count_threads < argumments->numThreads)
    {
        // printf("perimenw %ld\n", pthread_self());
        pthread_cond_wait (&cv , &mtx);
    }

    // printf("vgika %ld %s\n", pthread_self(), argumments->command);

    // Unlock mutex 
    if (err = pthread_mutex_unlock(&mtx)) 
    { 
        perror2("pthread_mutex_unlock", err); 
        exit (1); 
    }

    // Create socket
    if ((sock = socket( AF_INET , SOCK_STREAM , 0)) < 0)
        perror_exit ( " socket " ) ;

    server.sin_family = AF_INET ; // Internet domain   
    inet_aton(argumments->serverIP, &server.sin_addr);  // Server IP 
    server.sin_port = htons(argumments->serverPort); // Server port

    // Initiate connection 
    if (connect(sock, serverptr, sizeof(server)) < 0)
        perror_exit ("connect") ;
    
    struct hostent * foundhost = gethostbyaddr((const char *) &server.sin_addr , sizeof(server.sin_addr) , AF_INET ) ;
    // printf ( " Connecting to %s port %d\n" , foundhost->h_name , argumments->serverPort ) ;

    // Writing command on socket
    write_number_of_bytes(argumments->bytesOfCommand, argumments->bytesOfCommand, sock);
    write_message(argumments->command, argumments->bytesOfCommand, argumments->bytesOfCommand, sock);    
    
    // Read bytes of next message
    if ( (nread = read(sock, total_bytes, sizeof(int))) < 0 )
    {
        perror ("problem in reading");
        exit(5);
    }

    // Read message and print to output
    temp_str = malloc(sizeof(char) * (*(int *) total_bytes));
    
    if ( (nread = read(sock, temp_str, *(int *)total_bytes)) < 0 )
    {
        perror ("problem in reading");
        exit(5);
    }

    // Lock mutex for printing to stdout
    if (err = pthread_mutex_lock(&mtx_stdout))
    {
        perror2("pthread_mutex_lock", err); 
        exit(1); 
    }  
      
    printf("%s", temp_str);

    // Unlock mumtex for printing to stdout
    if (err = pthread_mutex_unlock(&mtx_stdout)) 
    { 
        perror2("pthread_mutex_unlock", err); 
        exit (1); 
    }    

    free(temp_str);
    free(total_bytes);
    close(sock);     
    pthread_exit(NULL);    
}

void initialize_thread_whoClient_Params(char * serverIP, int serverPort, char * command, int numThreads,
                                        int bytes, struct thread_whoClient_Params * params)
{
    params->serverIP = serverIP;
    params->serverPort = serverPort;
    params->numThreads = numThreads;
    params->command = command;
    params->bytesOfCommand = bytes;
}