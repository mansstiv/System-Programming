#include "masterFunc.h"


void listCountries (char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2])
{
	int total_bytes = strlen(buffer) + 1;
	char * message = NULL;

	for (int i = 0; i < numWorkers; ++i)
	{
		if (fd[i][1] == -2)
			break;

		write_number_of_bytes(total_bytes, bufferSize, fd[i][1]);
		write_message(buffer, total_bytes, bufferSize, fd[i][1]);		
	}

	read_message_with_select(numWorkers, fd, bufferSize, &message);
	printf("%s", message);
	free(message);
}



void diseaseFrequency(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
						char ** countries_path, int numCountries, int * success, int * fail )
{

	int total_bytes = strlen(buffer) + 1, workerID;
	char * country = NULL;
	char * date1, * date2;
	char * temp, * message = NULL;
	char temp_str[500];

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " "); // Points to disease name

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);

	temp = strtok(NULL, " ");

	// Check if date1 <= date2
	if (compareDates(date1, date2) > 0)
	{
		printf("Invalid date parameters: %s > %s\n", date1, date2);
		free(date1); 
		free(date2);
		(*fail)++;
		return;
	}
	else
	{
		//  Didn't give country parameter
		if (temp == NULL)
		{	
			int total_cases = 0;

			// So send command to all workers
			for (int i = 0; i < numWorkers; ++i)
			{
				if (fd[i][1] == -2)
					break;

				write_number_of_bytes(total_bytes, bufferSize, fd[i][1]);
				write_message(buffer, total_bytes, bufferSize, fd[i][1]);	
			}

			read_message_with_select(numWorkers, fd, bufferSize, &message);
			strcpy(temp_str, message);
			free(message);

			// Counting total number of cases for disease
			temp = strtok(temp_str, "\n");

			while (temp != NULL)
			{	
				total_cases += atoi(temp);
				temp = strtok(NULL, "\n");
			}

			printf("%d\n", total_cases);
			
		}
		// User gave country parameter
		else
		{
			// Initialize country
			country = malloc(strlen(temp) + 1);
			strcpy(country, temp);

			// So send command only to worker is responsible for given country
			workerID = country_which_worker(countries_path, temp, numWorkers, numCountries);
			
			if (workerID == -1)
			{
				(*fail)++;
				printf("Invalid country\n");
				// Deallocate space
				free(date1); 
				free(date2);
				if (country != NULL)
					free(country);
				return;
			}

			write_number_of_bytes(total_bytes, bufferSize, fd[workerID][1]);
			write_message(buffer, total_bytes, bufferSize, fd[workerID][1]);	
			
			total_bytes = read_number_of_bytes_of_next_message(bufferSize, fd[workerID], NULL, NULL, NULL);
			read_message(&message, total_bytes, bufferSize, fd[workerID][0]);
			printf("%s", message);
			free(message);
		}
		
	}	

	free(date1);
	free(date2);
	if (country != NULL)
		free(country);
	(*success)++;	
}

void topkAgeRanges(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
							char ** countries_path, int numCountries, int * success, int * fail)

{
	int total_bytes = strlen(buffer) + 1, workerID, k;
	char * country = NULL;
	char * date1, * date2;
	char * temp, * message = NULL;
	char temp_str[500];

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");

	// Initialize k	
	temp = strtok(NULL, " "); // Points to k
	k = atoi(temp);	

    if (k <= 0)
    {
		(*fail)++;
        printf("Invalid k (must be > 0)\n");
        return;
    }

	// Initialize country
	temp = strtok(NULL, " ");
	country = malloc(strlen(temp) + 1);
	strcpy(country, temp);	

	// So send command only to worker is responsible for given country
	workerID = country_which_worker(countries_path, temp, numWorkers, numCountries);
	
	if (workerID == -1)
	{
		printf("Invalid country\n");
		// Deallocate space
		if (country != NULL)
			free(country);
		(*fail)++;	
		return;
	}

	temp = strtok(NULL, " "); // Points to disease name

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);

	// Check if date1 <= date2
	if (compareDates(date1, date2) > 0)
	{
		printf("Invalid date parameters: %s > %s\n", date1, date2);
		free(date1); 
		free(date2);
		if (country != NULL)
			free(country);
		(*fail)++;	
		return;
	}
	else
	{
		write_number_of_bytes(total_bytes, bufferSize, fd[workerID][1]);
		write_message(buffer, total_bytes, bufferSize, fd[workerID][1]);	
		
		total_bytes = read_number_of_bytes_of_next_message(bufferSize, fd[workerID], NULL, NULL, NULL);
		read_message(&message, total_bytes, bufferSize, fd[workerID][0]);
		printf("%s", message);
		free(message);
		
	}	

	free(date1);
	free(date2);
	if (country != NULL)
		free(country);	
	(*success)++;	
}							

void numPatientAdmissions(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
							char ** countries_path, int numCountries, int * success, int * fail )
{

	int total_bytes = strlen(buffer) + 1, workerID;
	char * country = NULL;
	char * date1, * date2;
	char * temp, * message = NULL;
	char temp_str[500];

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " "); // Points to disease name

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);

	temp = strtok(NULL, " ");

	// Check if date1 <= date2
	if (compareDates(date1, date2) > 0)
	{
		printf("Invalid date parameters: %s > %s\n", date1, date2);
		free(date1); 
		free(date2);
		(*fail)++;
		return;
	}
	else
	{
		//  Didn't give country parameter
		if (temp == NULL)
		{	
			int total_cases = 0;

			// So send command to all workers
			for (int i = 0; i < numWorkers; ++i)
			{
				if (fd[i][1] == -2)
					break;

				write_number_of_bytes(total_bytes, bufferSize, fd[i][1]);
				write_message(buffer, total_bytes, bufferSize, fd[i][1]);	
			}

			read_message_with_select(numWorkers, fd, bufferSize, &message);
			printf("%s", message);
			free(message);
		}
		// User gave country parameter
		else
		{
			// Initialize country
			country = malloc(strlen(temp) + 1);
			strcpy(country, temp);

			// So send command only to worker is responsible for given country
			workerID = country_which_worker(countries_path, temp, numWorkers, numCountries);
			
			if (workerID == -1)
			{
				printf("Invalid country\n");
				// Deallocate space
				free(date1); 
				free(date2);
				if (country != NULL)
					free(country);
				(*fail)++;	
				return;
			}

			write_number_of_bytes(total_bytes, bufferSize, fd[workerID][1]);
			write_message(buffer, total_bytes, bufferSize, fd[workerID][1]);	
			
			total_bytes = read_number_of_bytes_of_next_message(bufferSize, fd[workerID], NULL, NULL, NULL);
			read_message(&message, total_bytes, bufferSize, fd[workerID][0]);
			printf("%s", message);
			free(message);
		}
		
	}	

	free(date1);
	free(date2);
	if (country != NULL)
		free(country);
	(*success)++;	
}

void searchPatientRecord(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2])
{
	int total_bytes = strlen(buffer) + 1;
	char * message = NULL, * token;
	char temp_str[500];
	int flag = 0;

	for (int i = 0; i < numWorkers; ++i)
	{
		if (fd[i][1] == -2)
			break;

		write_number_of_bytes(total_bytes, bufferSize, fd[i][1]);
		write_message(buffer, total_bytes, bufferSize, fd[i][1]);		
	}	

	read_message_with_select(numWorkers, fd, bufferSize, &message);
	strcpy(temp_str, message);
	token = strtok(temp_str, "\n");
	while(token != NULL)
	{
		// "-" answered workers that didn't found patient so ignore it
		if (strcmp(token, "-") != 0)
		{
			printf("%s\n", token);
			flag = 1;
		}
		token = strtok(NULL, "\n");
	}

	if (flag == 0)
		printf("Didn't find patient\n");
	
	free(message);

}

void numPatientDischarges(char * buffer, int bufferSize, int numWorkers, int fd[numWorkers][2], 
							char ** countries_path, int numCountries, int * success, int * fail)

{
	int total_bytes = strlen(buffer) + 1, workerID;
	char * country = NULL;
	char * date1, * date2;
	char * temp, * message = NULL;
	char temp_str[500];

	strcpy(temp_str, buffer);	
	temp = strtok(temp_str, " ");
	temp = strtok(NULL, " "); // Points to disease name

	// Initialize date1
	temp = strtok(NULL, " "); 
	date1 = malloc(strlen(temp) + 1);
	strcpy(date1, temp);

	// Initialize date2
	temp = strtok(NULL, " ");
	date2 = malloc(strlen(temp) + 1);
	strcpy(date2, temp);

	temp = strtok(NULL, " ");

	// Check if date1 <= date2
	if (compareDates(date1, date2) > 0)
	{
		printf("Invalid date parameters: %s > %s\n", date1, date2);
		free(date1); 
		free(date2);
		(*fail)++;
		return;
	}		
	else
	{
		//  Didn't give country parameter
		if (temp == NULL)
		{	
			int total_cases = 0;

			// So send command to all workers
			for (int i = 0; i < numWorkers; ++i)
			{
				if (fd[i][1] == -2)
					break;

				write_number_of_bytes(total_bytes, bufferSize, fd[i][1]);
				write_message(buffer, total_bytes, bufferSize, fd[i][1]);	
			}

			read_message_with_select(numWorkers, fd, bufferSize, &message);
			printf("%s", message);
			free(message);
		}
		// Country parameter is given
		else
		{
			// Initialize country
			country = malloc(strlen(temp) + 1);
			strcpy(country, temp);

			// So send command only to worker is responsible for given country
			workerID = country_which_worker(countries_path, temp, numWorkers, numCountries);
			
			if (workerID == -1)
			{
				printf("Invalid country\n");
				// Deallocate space
				free(date1); 
				free(date2);
				if (country != NULL)
					free(country);
				(*fail)++;		
				return;
			}

			write_number_of_bytes(total_bytes, bufferSize, fd[workerID][1]);
			write_message(buffer, total_bytes, bufferSize, fd[workerID][1]);	
			
			total_bytes = read_number_of_bytes_of_next_message(bufferSize, fd[workerID], NULL, NULL, NULL);
			read_message(&message, total_bytes, bufferSize, fd[workerID][0]);
			printf("%s", message);
			free(message);
		}
		
	}	

	free(date1);
	free(date2);
	if (country != NULL)
		free(country);	
	(*success)++;
}

/////////////////////////////////// Signals ///////////////////////////////////

void parenthandler_sigint_sigquit(int signo)
{
	parent_sigint_sigquit = 1; // Set flag
}

void parenthandler_sigchld(int signo)
{
	parent_sigchld = 1; // Set flag
}

void parenthandler_sigusr1(int signo)
{
	parent_sigusr1 = 1; // Set flag
}

void exit_with_sigint_sigquit(char ** countries_path, int countries_count, int numWorkers, int workers_pids[numWorkers],
								int * success, int * fail)
{

    char *name;
	FILE* fp;

    name = malloc(strlen("log_file.") + count_digits(getpid()) + 1);
    sprintf(name, "log_file.%d", getpid());
    fp = fopen(name, "w+");

	for (int i = 0; i < countries_count; ++i)
		 fprintf(fp, "%s\n", get_country_from_path(countries_path[i]));

    fprintf(fp, "TOTAL %d\n", *success + *fail);
    fprintf(fp, "SUCCESS %d\n", *success);
    fprintf(fp, "FAIL %d\n", *fail);
    
    free(name);
    fclose(fp);    

	for (int i = 0; i < numWorkers; ++i)
	{
		if (workers_pids[i] != -1);
			kill(workers_pids[i], SIGKILL);
	}

	int status;
	pid_t wpid;
	while ((wpid = wait(&status)) > 0);
}

void sigchld_function(int numWorkers, int workers_pids[numWorkers], char ** countries_path_for_workers, int * worker_bytes,
						int fd[numWorkers][2], int bufferSize)
{

	int status, workerID, bytes_to_read;
	pid_t pid;
	char total_bytes[12]; // Total bytes each worker must read 
    char buffer_size[12]; // Size of pipe
    char worker_ID[10];
	char * temp_str;
	int dead_pid;


	dead_pid = waitpid(-1, &status, WNOHANG); // pid of dead child

	if (dead_pid <= 0) 
		return;

	for (int i = 0; i < numWorkers; ++i)
	{
		if (workers_pids[i] == dead_pid)
		{
			workerID = i;
			break;
		}
	}

	if ((pid = fork()) == -1)
	{
		perror (" fork "); 
		exit (1);
	}	

	if (pid > 0)
	{
		workers_pids[workerID] = pid;
		write_message(countries_path_for_workers[workerID], worker_bytes[workerID], bufferSize, fd[workerID][1]);
		bytes_to_read = read_number_of_bytes_of_next_message(bufferSize, fd[workerID], NULL, NULL, NULL);
		read_message(&temp_str, bytes_to_read, bufferSize, fd[workerID][0]);
		// printf("%s", temp_str);
		free(temp_str);
	}
	else
	{
		sprintf(buffer_size, "%d", bufferSize);
		sprintf(worker_ID,"%d",workerID);
		sprintf(total_bytes, "%d", worker_bytes[workerID]);
		char *args[] = {"./worker", worker_ID, total_bytes, buffer_size, NULL};
		execvp("./worker", args);            		
	}

}						

void sigurs1_function(int numWorkers, int fd[numWorkers][2], int bufferSize)
{
	int retval, max_fd = 0, totalBytes;
	char * msgbuf, * message = NULL;
	fd_set rfds;
    

    // Reading statistics from workers
	FD_ZERO(&rfds);

	for (int i = 0; i < numWorkers; ++i)
	{
		if (fd[i][0] != -2)
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
			if (fd[i][0] == -2)
				continue;

			if (FD_ISSET(fd[i][0], &rfds)) 
			{
				totalBytes = read_number_of_bytes_of_next_message(bufferSize, fd[i], NULL, NULL, NULL);
				read_message(&msgbuf, totalBytes, bufferSize, fd[i][0]);
				my_strcat(&message, msgbuf);  
				free(msgbuf);    
			}
		}

	}
	// No data found
	else
		printf("There is no data\n");

	printf("%s", message);
	free(message);	
}