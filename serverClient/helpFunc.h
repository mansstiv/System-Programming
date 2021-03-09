#include "workerStructs.h"
// #include "workerFunc.h"

DIR * sudo_opendir(char *name);

char * sort_date_files(char *name, int *num);

unsigned int hash(char *str, unsigned int HASHSIZE);

void uber_print(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries); // For debugging purposes

void my_strcat(char ** str1, char * str2);

int get_case_num(char * buffer);

int count_digits(int num);

void write_number_of_bytes(int bytes, int bufferSize, int fd);

// Returns number of bytes of next message
int read_number_of_bytes_of_next_message(int bufferSize, int fd[2], 
                    struct subdirectory_list * subdirectory_list, int * success, int * fail); 

void write_message(char * message, int bytes_to_write, int bufferSize, int fd);

// Reads message and stores it to msgbuf array
void read_message(char ** msgbuf, int bytes_to_read, int bufferSize, int fd);

void read_message_with_select(int numWorkers, int fd[numWorkers][2], int bufferSize, char ** message);

// Inspired by http://jhshi.me/2013/11/02/use-select-to-monitor-multiple-file-descriptors/index.html
void sudo_fd_set(int fd, fd_set * fds, int * max_fd);

char * get_country_from_path(char * path);

// Takes all countries
// Returns worker id that is responsible for this country
// If no worker is responsible for this country -1 is returned
int country_which_worker(char ** countries_path, char * country, int numWorkers, int numCountries);

void create_log_file_worker(struct subdirectory_list * subdirectory_list, int * success, int * fail);

void function_sigusr1(struct subdirectory_list * subdirectory_list, int bufferSize, int fd);

void error_checking(struct RBTree * T, struct node * node, int flag_delete);

void insert_to_tree(struct subdirectory * subdirectory, struct RBTree * T, struct node * node);

void insert_patients_to_structs(struct RBTree * T, struct node * node, struct subdirectory * subdirectory,
                                int bucketSize, int diseaseHashtableNumOfEntries);

void create_statistics(struct RBTree * T, char * date, char * country, char ** retString);      

void perror_exit (char * message);
