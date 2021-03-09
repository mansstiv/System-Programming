STIVAKTAS EMMANOUIL
AM: 1115201700152

All questions have been answered and have been implemented according the exercise requirements. The points that need further explanation are the following:


SOME INFORMATION ABOUT THE STRUCTS BEING USED:

    ( The logic of structs is the same as in the 1st exercise. RBTree for patients, diseaseHashtTables, buckets ... )
    

    1)	The implementation of RBT is exactly as described in the book "Introduction to Algorithms", 3d edition. 
	    Critical point of this implementation is that the tree nodes do not point to NULL, but to a guard node, pointing to NULL. 
	    This makes it easier to handle the nodes and more generally the operations in the RBT.

    2)  Struct patientRecord keeps informations for our patients, who are stored in a list.

    3)  For topk question i implemented a heap with a tree . Each node of the tree points to a node of a list that holds 
        a number of cases according to the topk question . 

    4)  For bucket i used struct bucket that consists of multiple bucket entries (struct bucketEntry). If all bytes (bucketSize parameter) of bucket are 
        being used new bucket will dynamically be allocated.

    5) I used some global flag variables for signals. According to their value (0 or 1 : handler for each signal the responsible for changing the 
       corresponding flag value) program knows if a signal was caught.

    6) Each worker has a list (struct subdirectory_list) of subdirectories (struct subdirectory) that holds up some useful info's like countries paths,
       date_files, RBTree with all patients of country etc...


OTHER USEFUL INFORMATION / ASSUMPTIONS TAKEN

        1) Communication protocol: 
                
                - When someone wants to send a message to the suitable pipe, first he sends total number of bytes of the upcoming message 
                   and then the actual message . In this way, reader know how many bytes to read. 
                  
                - For each write i send one message that holds up all the informations.
        
                - With the help of select() parent doesn't get stuck waiting for a slow worker to send informations. He always reads first messages to come.
                
                - After statistics being sent and read the general logic is:
                        
                        a) Parent waits for input. 
                        
                        b) He writes number of bytes of command and actual command to suitable worker/workers and then waits for answer.
                        
                        c) Worker reads command and sends back number of bytes of upcoming message and the actual message.
                        
                        d) Parent reads message and prints it in stdout.
        
        
        2) If you want to send SIGUSR1 signal to a worker, send afterwards SIGUSR1 to parent. In this way parent waits for message (statistics) by the workers
            and not by the user.

        3) I send errors for invalid patients in stdout , but there are currently in comments. You can uncomment them. 
            You can find them in : (helpFunc.c -> void error_checking ().
        
        4) TOTAL, SUCCESS and FAIL (How they are counted)
            
                Parent: 
                        FAIL : if wrong command , invalid date, invalid country
                    
                        else SUCCESS
                    
                Worker:
                        Fail : if wrong disease, or if patient wasn't found
                        
                        else SUCCESS                    
                
SOURCES :

    1)	Informations from sources on the web were used to implement the hash function. I also consulted the internet for finding the most right element 
        in a tree with a shift method . Generally, there are comments on these functions, referring to the corresponding sources used.


HOW TO RUN: 

    First run bash script as:
    
        ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile

    
    Then there is a makefile that responds to the following commands:

	    1)	make ->compile the program and produce the corresponding object files and the executable.

	    2)	make clean -> deletes the object files and executable that were created.
    
        ALWAYS, after you exit program and if you want to EXECUTE IT AGAIN first MAKE CLEAN and then MAKE again.
       
        --->  ./diseaseAggregator -w numWorkers -b bufferSize -i input_dir  
        
        Global variables bucketSize and diseaseHashtableNumOfEntries are set to 100 and 10 accordingly. If you want to change them you have to apply the values in
        helpFunc.c (line 3, 4).

        My programm does support defensive programming. However, it will respond only in input errors based on the requirements of the exercise. 
        For example, if user changes the correct order of parameters given (for each command), the programm will not function correctly, etc...

FILES :
    
    1) patientRecord.c, patientRecord.h // For struct patientRecord
    
    2) topk.c, topk.h // For topk questions. Includes a heap and a list implementation used for these 2 questions

    3) RBTree.c, RBTree.h   // Red black tree implementation

    4) bucket.c, bucket.h // Bucket and bucketEntry implementation

    5) helpFunc.c , helpFunc.h // Some general helpfull functions used throughout the programm. There you will find also reading and writing functions

    6) headers.h // All headers needed

    7) worker.c // Main of worker
    
    8) workerFunc.c, workerFunc.h // Functions of worker. Mainly functions used for actual questions and some signals handlers

    9) workerStructs.c, workerStructs.h // Some struct implementation for worker    
    
    10) parentProccess.c // Main of parent

    11) parentFunc.c, parentFunc.h // Functions of parent. Mainly functions used for actual questions and some signals handlers

    




           
                

        

