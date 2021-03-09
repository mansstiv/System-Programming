# System-Programming

## Part1
Purpose of ths project is to implement a programm, which based on an input file, can extract informations and can answer different questions about virus cases. 

### Structures
1. Hash Tables
2. Linked lists
3. Rea d Black Trees

### Why these structures?
Due to the big amount of patient records entries, hash tables and red black trees can secure a small time complexity on performing specific queries.

### How to run..
There is a makefile that responds to the following commands:

  1. ```make``` -> compile the program and produce the corresponding object files and executable.

  2. ```make clean``` -> deletes the object files and executable that were created.

```./diseaseMonitor -p patientRecordsFile -h1 diseaseHashtableNumOfEntries -h2 countryHashtableNumOfEntries -b bucketSize```

NOTE : bucketSize must be >= 24 (because bucket must have at least 1 bucketEntry and 1 pointer which sum up to 24 bytes)

An example: h1 = 10, h2 = 10, bucketSize = 100.   

### Commands

* ```/globalDiseaseStats [date1 date2]```

* ```/diseaseFrequency virusName [country] date1 date2```

* ```/topk-Diseases k country [date1 date2]```

* ```/topk-Countries k disease [date1 date2]```

* ```/insertPatientRecord recordID patientFirstName patientLastName diseaseID entryDate [exitDate]```

* ```/recordPatientExit recordID exitDate```

* ```/numCurrentPatients [disease]```

* ```/exit```
