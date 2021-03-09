STIVAKTAS EMMANOUIL
AM: 1115201700152

--> I have used code from my previous project, so instead of rewriting informations about structs and functions being used, you can find previous readme (proj2README.txt) in the folder.


All questions have been answered and have been implemented according the exercise requirements. The new points that need further explanation are the following:


→ For the synchronization of threads in whoServer and whoClient  I use global mutexes and conditional variables .

→ In whoServer I use the the following global structs:

    1) struct workers_info, struct ports_array : 

    Hold information about workers, their ip and their port. Useful for “rest numThreads” of whoServer that connect with workers. (basically work as servers now).

    2) struct circular_buffer, struct buffer_entry: 

    Circular buffer holds multiple socket file-descriptors. Fd’s are represented by struct buffer_entry which also helps us seperate fd’s that
    come from  workers and those who come from whoClients threads. 


How to run:
    
    1) First run create_infiles.sh script to creade subdirectory.

        ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile

    2) Then run whoServer . If queryPortNum and statisticsPortNum are being used you will get an error, so you have to reexecute whoServer till 
    you give ports that are open. 

    3) Afterwards run master.

    4) Finally run whoClient.

    After each execute, type “ make clean ” ( to delete executables, object file, named pipes...) then “ make “ .

For closing programs:

	whoServer: 
			ctrl+C signal is handled and memory is freed

	master: 
		
		first ctrl+C to kill worker procceces and then type “ /exit ” for 	master 	to close
  
Extra/different files:

	→ whoServer.c, whoServerFunc.c, whoServerFunc.h : About whoServer

	→ whoClient.c, whoClientFunc.c, whoClientFunc.h : About whoClient

    -> master.c , masterFunc.c, masterFunc.h : About master


In QueryFile: 

    You must give valid commands. Last line of file must be a command. If there are empty lines programm will defunction. Also at the end of each command do it’s 
    important not to have extra empty spaces. I have a queryFile sample.


Code is used in circular buffer from the slides of our class (functions obtain(), place(), and generally the logic and how it works) 




