#include "whoServerFunc.h"

volatile sig_atomic_t whoServer_sigint = 0;

pthread_mutex_t mtx_buffer; // Mutex for conditional variables cond_nonempty, cond_nonfull
pthread_mutex_t mtx_whoServer_stdout; // Mutex for printing to stdout
pthread_mutex_t mtx_whoSerer_variables; // Mutex for global variable worker_info

// Conditional variables for circular buffer
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

struct circular_buffer buffer;
struct workers_info workers_info;


void initialize_circular_buffer(struct circular_buffer * buffer, int bufferSize) 
{
	buffer->start = 0;
	buffer->end = -1;
	buffer->count = 0;
    buffer->data = malloc(sizeof(struct buffer_entry) * bufferSize);
}

// Places item to circular buffer
void place(struct circular_buffer * buffer, int data, int bufferSize, char * status) 
{
	pthread_mutex_lock(&mtx_buffer);
	while (buffer->count >= bufferSize) 
    {
		// printf("\n\n>> Found Buffer Full \n\n");
		pthread_cond_wait(&cond_nonfull, &mtx_buffer);
	}
	buffer->end = (buffer->end + 1) % bufferSize;
	buffer->data[buffer->end].fd = data;
    strcpy(buffer->data[buffer->end].status, status);
	buffer->count++;
	pthread_mutex_unlock(&mtx_buffer);
}

// Obtains item from circular buffer
int obtain(struct circular_buffer * buffer, int bufferSize, char status[]) 
{
	int data = 0;
	pthread_mutex_lock(&mtx_buffer);
	while (buffer->count <= 0) 
    {
		// printf(">> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx_buffer);
	}
	data = buffer->data[buffer->start].fd;
    strcpy(status, buffer->data[buffer->start].status);
	buffer->start = (buffer->start + 1) % bufferSize;
	buffer->count--;
	pthread_mutex_unlock(&mtx_buffer);
	return data;
}

// Stores ip of client to argument char ip[]
void get_ip (struct hostent * rem, char ip[])
{
    struct in_addr ** addr_list ;
    addr_list = (struct in_addr **) rem->h_addr_list;
    strcpy(ip, inet_ntoa(*addr_list[0]));    
}


// Main thread function
void * whoServer_thread(void * params)
{
    int sockfd, nread, retVal, num = 0;
    struct thread_whoServer_Params * argumments = params;
    void * total_bytes;
    char * temp_str = NULL, *temp_str2 = NULL, *answer = NULL, *final_answer = NULL;
    int sock;
    struct sockaddr_in server;
    struct sockaddr * serverptr = (struct sockaddr *) &server;        
    char status[50]; char temp_str3[100];
    int bytesOfCommands = 0, diseaseFreqVal=0;

	// while (buffer.count > 0) 
    while(1)
    {   
        sockfd = obtain(&buffer, argumments->bufferSize, status);

        total_bytes = malloc(sizeof(int));

        // Statistics case
        if (strcmp(status, "Statistics") == 0)
        {
            // Reading serverPort of worker
            if ( (nread = read(sockfd, total_bytes, sizeof(int))) < 0 )
            {
                perror ("problem in reading");
                exit(5);
            }         

            // Insert port to structs
            pthread_mutex_lock(&mtx_whoSerer_variables);  
            insert_port(&workers_info.ports_array, *(int *)total_bytes);             
            pthread_mutex_unlock(&mtx_whoSerer_variables);
            
            // Read message, print to stdout, locking and unlocking respective mutex 
            if ( (nread = read(sockfd, total_bytes, sizeof(int))) < 0 )
            {
                perror ("problem in reading");
                exit(5);
            }                           
            read_message(&temp_str, *(int *)total_bytes, *(int *)total_bytes, sockfd);
            
            pthread_mutex_lock(&mtx_whoServer_stdout);
            printf("%s\n", temp_str);    
            free(temp_str);
            pthread_mutex_unlock(&mtx_whoServer_stdout);  
        }    
        
        // Query case
        else if (strcmp(status, "Query") == 0)
        {   
            // Reading commands 
            if ( (nread = read(sockfd, total_bytes, sizeof(int))) < 0 )
            {
                perror ("problem in reading");
                exit(5);
            }   

            read_message(&temp_str, *(int *)total_bytes, *(int *)total_bytes, sockfd);
            bytesOfCommands = *(int *)total_bytes;
            temp_str2 = temp_str;

            // Loop through all possible commands
            while (1)
            {
                sprintf(temp_str3, "\n\nCOMMAND: %s\n\n", temp_str2);
                my_strcat(&final_answer, temp_str3);
                
                diseaseFreqVal = 0;

                // Loop for all workers
                for (int i = 0; i < workers_info.ports_array.size; ++i)
                {
                    // Create socket
                    if ((sock = socket( AF_INET , SOCK_STREAM , 0)) < 0)
                        perror_exit ( " socket " ) ;

                    server.sin_family = AF_INET ; // Internet domain   
                    inet_aton(workers_info.ip, &server.sin_addr);  // Server IP 
                    server.sin_port = htons(workers_info.ports_array.array[i]); // Server port

                    // Initiate connection 
                    if (connect(sock, serverptr, sizeof(server)) < 0)
                        perror_exit ("connect") ;  

                    // Write command to worker
                    write_number_of_bytes(strlen(temp_str2)+1, sizeof(int), sock);
                    write_message(temp_str2, strlen(temp_str2)+1, strlen(temp_str2)+1, sock); 

                    // Reading answer of worker 
                    if ( (nread = read(sock, total_bytes, sizeof(int))) < 0 )
                    {
                        perror ("problem in reading");
                        exit(5);
                    }  
                    read_message(&answer, *(int *)total_bytes, *(int *)total_bytes, sock);
                    
                    // Check if command is diseaseFreequency
                    if (answer[0] >= '0' && answer[0] <= '9' && strchr(answer, ':') == NULL && strchr(answer, '-') == NULL)
                    {
                        answer[strlen(answer) - 1] = '\0';
                        diseaseFreqVal += atoi(answer);
                        // last worker return so write to final_answer total num
                        if (i == workers_info.ports_array.size - 1)
                        {
                            char value[count_digits(diseaseFreqVal)+1];
                            sprintf(value, "%d\n", diseaseFreqVal);
                            my_strcat(&final_answer, value);
                        }   
                    }
                    else if (strcmp(answer, "-\n") != 0) // Ignore this string
                            my_strcat(&final_answer, answer);
                    
                    free(answer);
                    close(sock);      
                }

                // Print answer to stdout
                pthread_mutex_lock(&mtx_whoServer_stdout);
                printf("%s", final_answer);    
                pthread_mutex_unlock(&mtx_whoServer_stdout);  

                num += strlen(temp_str2) + 1;        
                if (num >= bytesOfCommands)
                    break;     
                temp_str2 = &temp_str[num];                   
            }

            // Write answer back to whoClient
            write_number_of_bytes(strlen(final_answer) + 1, strlen(final_answer) + 1, sockfd);
            write_message(final_answer, strlen(final_answer) + 1, strlen(final_answer) + 1, sockfd);
            
            free(final_answer);                            
            free(temp_str);
        }

        free(total_bytes);
        pthread_cond_signal(&cond_nonfull);
	}    
}

void initialize_thread_whoServer_Params(int bufferSize, struct thread_whoServer_Params * params)
{
    params->bufferSize = bufferSize;
}

void initialize_workers_info(struct workers_info * workers_info)
{
    initialize_ports_array(&workers_info->ports_array);
    workers_info->ip = NULL;
}

void initialize_ports_array (struct ports_array * ports_array)
{
  ports_array->array = NULL;
  ports_array->size = 0;
}

void insert_port (struct ports_array * ports_array, int port)
{
  if (ports_array->array == NULL)
  {
    ports_array->array = malloc(sizeof(int));
    ports_array->array[0] = port;
    ports_array->size = 1;
  }
  else
  {
    int new_size = ports_array->size * sizeof(int) + sizeof(int);
    ports_array->array = realloc(ports_array->array, new_size);
    memmove(ports_array->array+ports_array->size, &port, sizeof(int)); 
    ports_array->size++;
  }
}

void whoServer_handler_sigint(int signo)
{
    whoServer_sigint = 1; // Set flag
}