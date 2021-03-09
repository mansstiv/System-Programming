#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/wait.h> 
#include <unistd.h>  
#include <errno.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>

struct circular_buffer;

#define MAX_LEN 100
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

extern int count_threads;
extern pthread_mutex_t mtx; // Mutex for conditional variable
extern pthread_mutex_t mtx_stdout; // Mutex for printing to stdout
extern pthread_cond_t cv; // Condition variable

extern pthread_mutex_t mtx_buffer;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;
extern pthread_mutex_t mtx_whoServer_stdout;
extern pthread_mutex_t mtx_whoSerer_variables;
extern struct circular_buffer buffer;
extern struct workers_info workers_info;