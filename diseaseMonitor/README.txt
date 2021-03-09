STIVAKTAS EMMANOUIL
AM: 1115201700152

The questions have been implemented according the exercise requirements. The points that need further explanation are the following:


SOME INFORMATION ABOUT THE STRUCTS BEING USED:

    1)	The implementation of RBT is exactly as described in the book "Introduction to Algorithms", 3d edition. 
	    Critical point of this implementation is that the tree nodes do not point to NULL, but to a guard node, pointing to NULL. 
	    This makes it easier to handle the nodes and more generally the operations in the RBT.

    2)  Struct patientRecord keeps informations for our patients, who are stored in a list.

    3)  For topk questions i implemented a heap with a tree . Each node of the tree points to a node of a list that holds 
        a number of cases according to the topk question . 

    4)  For bucket i used struct bucket that consists of multiple bucket entries (struct bucketEntry). If all bytes (bucketSize parameter) of bucket are 
        being used new bucket will dynamically be allocated.

    5) I use a global variable (int count) , that helps counting the nodes of RBTree while traversing it.

   

SOURCES :

    1)	Informations from sources on the web were used to implement the hash function. I also consulted the internet for finding the most right element 
        in a tree with a shift method . Generally, there are comments on these functions, referring to the corresponding sources used.


HOW TO RUN: 

    There is a makefile that responds to the following commands:

	    1)	make ->compile the program and produce the corresponding object files and the executable.

	    2)	make clean -> deletes the object files and executable that were created.
       
    ./diseaseMonitor -p patientRecordsFile -h1 diseaseHashtableNumOfEntries -h2 countryHashtableNumOfEntries -b bucketSize

    NOTE : bucketSize must be >= 24 (because bucket must have at least 1 bucketEntry and 1 pointer which sum up to 24 bytes)
    
    An example: h1 = 10, h2 = 10, bucketSize = 100    
    
    My programm does support defensive programming. However, it will respond only in input errors based on the requirements of the exercise. 
    For example, if user changes the correct order of parameters given (for each command), the programm will not function correctly, etc...

FILES :
    
    1) patientRecord.c, patientRecord.h // For struct patientRecord
    
    2) topk.c, topk.h // For topk questions. Includes a heap and a list implementation used for these 2 questions

    3) RBTree.c, RBTree.h   // Red black tree implementation

    4) list.c, list.h // Implementation of main list , which holds patients info

    5) bucket.c, bucket.h // Bucket and bucketEntry implementation

    6) helpFunc.c , helpFunc.h // Some extra helpfull functions used throughout the programm

    7) mainFunc.c , mainFunc.h // 8 functions for the 8 main commands a user can type 

    8) main.c 

VALIDATOR : 

    1) All the test are succesful . 
    
    NOTE:
    
        -->  In 8_input_command (/diseaseFrequency EVD 01-01-2000 20-03-2020 China) the result shoud be EVD 1. 
        
        -->  My output BASED ON THE VALIDATOR is EVD 10, but the test is succesful at this point, because it searches for 
             EVD 1 which exists . You can verify it with the following command :

              " cat 8_input_command_RND10244096.cmd | ./diseaseMonitor -p small.txt -h1 10 -h2 10 -b 100 | head -20 "

        -->  This happens, because i follow the following order of parameters based on the requirements of the exercise :

              " /diseaseFrequency virusName [country] date1 date2 " 

        -->  If you run:  /diseaseFrequency EVD China 01-01-2000 20-03-2020 the output will be correct .
           
                

        

