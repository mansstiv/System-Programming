#include "workerFunc.h"

int main(int argc, char const *argv[])
{
    // Set signals
    // signal(SIGINT, workerhandler_sigint_sigquit);
    // signal(SIGQUIT, workerhandler_sigint_sigquit);
    // signal(SIGUSR1, workerhandler_sigusr1);

    int workerID = atoi(argv[1]);
    int total_bytes = atoi(argv[2]); // Total bytes for reading , writting
    int current_bytes = 0; // Current bytes read
                        // Must sum up to "bytes" to finish reading
    int temp_bytes, tempVal, serverPort=-1;                    
    int bufferSize = atoi(argv[3]); // Size of pipe                    
    char *msgbuf, *string, *new_string, *serverIP;
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


    // Read serverIP and serverPort
    tempVal = read_number_of_bytes_of_next_message(bufferSize, fd, NULL, NULL, NULL);
    read_message(&serverIP, tempVal, bufferSize, fd[0]);

    // Read serverPort
    serverPort = read_number_of_bytes_of_next_message(bufferSize, fd, NULL, NULL, NULL);

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

    

    int err, sock;
    struct sockaddr_in server;
    struct sockaddr * serverptr = (struct sockaddr *) &server;   

    // Create socket to send statistics to whoServer
    if ((sock = socket( AF_INET , SOCK_STREAM , 0)) < 0)
        perror_exit ( " socket " ) ;

    server.sin_family = AF_INET ; // Internet domain   
    inet_aton(serverIP, &server.sin_addr);  // Server IP 
    server.sin_port = htons(serverPort); // Server port

    // Initiate connection 
    if (connect(sock, serverptr, sizeof(server)) < 0)
        perror_exit ("connect") ;

    int sock2, newsock;
    struct sockaddr_in server2, client;
    struct sockaddr * server2ptr = (struct sockaddr *) &server2, * clientptr = (struct sockaddr *) &client;       
    socklen_t clientlen = sizeof(client);

    // Creating new socket 
    // Now listening for commands on this socket
    // Reading commands worker will send and answering back 
    if ((sock2 = socket( AF_INET , SOCK_STREAM , 0)) < 0)
        perror_exit ( " socket " ) ;

    server2.sin_family = AF_INET ; // Internet domain   
    server2.sin_addr.s_addr = htonl(INADDR_ANY);
    // server2.sin_port = htons(1100+workerID); // Server port     
    server2.sin_port = 0;    
    // server2.sin_port = htons(1052); // Server port   
    if (bind(sock2, server2ptr, sizeof(server2)) < 0)    
        perror_exit("bind");          
    
    // Listening for connections
    if (listen(sock2 , 1000) < 0) 
        perror_exit("listen");

    // Writing port on socket
    int myPort = get_port(sock2);
    write_number_of_bytes(myPort, sizeof(int), sock);

    // Writing statistics on socket
    total_bytes = strlen(new_string) + 1; 
    write_number_of_bytes(total_bytes, sizeof(int), sock);
    write_message(new_string, total_bytes, total_bytes, sock);    
    close(sock); 

	// printf("Local port : %u\n", myPort);    
    printf ("Listening for connections to port %d\n", myPort);

    free(msgbuf);

    while(1)
    {

        // accept connection 
        if ( (newsock = accept(sock2, clientptr, &clientlen)) < 0) 
            perror_exit("accept");  


        switch(fork())
        {
            case -1:    // error case
                perror("fork"); break;
            case 0: // child case
                close(sock2); worker_server(newsock, bufferSize, fd, subdirectory_list, diseaseHashtableNumOfEntries, new_string,
                                        serverIP, bufferPatientFirstName, bufferPatientLastName, bufferDiseaseID, bufferStatus);
                exit(0);
            // parent case
            close(newsock); 
        } 




    }

}