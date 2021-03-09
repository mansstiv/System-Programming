# include "helpFunc.h"

// Struct that has all ports being sent by workers to whoServer
struct ports_array {
  int * array;
  int size;
};

// General information about workers
struct workers_info {
	char * ip; // IP of workers
	struct ports_array ports_array; // An array with ports of workers
};

// Represents an fd to the circular_buffer
// char status helps us to seperate statistcs by the queries
struct buffer_entry {
	int fd;
	char status[50]; // Statistics or Query
};

// Circluar buffer 
struct circular_buffer{
	struct buffer_entry * data;
	int start;
	int end;
	int count;
} ;

// Parameters of thread function
struct thread_whoServer_Params {
	int bufferSize;
};

///////////////////////////////////////  Functions  ///////////////////////////////////////

void initialize_circular_buffer(struct circular_buffer * buffer, int bufferSize) ;

// Places item to circular buffer
void place(struct circular_buffer * buffer, int data, int bufferSize, char * status);

// Obtains item from circular buffer
int obtain(struct circular_buffer * buffer, int bufferSize, char status[]);

// Stores ip of client to argument char ip[]
void get_ip (struct hostent * rem, char ip[]);

// Main thread function
void * whoServer_thread(void * params);

void initialize_thread_whoServer_Params(int bufferSize, struct thread_whoServer_Params * params);

void initialize_workers_info(struct workers_info * workers_info);

void initialize_ports_array (struct ports_array * ports_array);

void insert_port (struct ports_array * ports_array, int port);

// Handler for sigint signal
void whoServer_handler_sigint(int signo);