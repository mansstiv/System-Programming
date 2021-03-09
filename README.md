# System-Programming

## Part1 
<p><b>Purpose</b> of this project is to implement a program, which based on an input file with patient records, can answer different questions about disease cases.</p>
<p><b>Tests</b> knowledge on <b>data structures</b> and <b>time complexity</b>.</p>

### Input
A [file](small.txt) with patient records entries. For example, a record could be ```889 Mary Smith COVID-2019 China 25-1-2019 27-1-2019```.

### Structures
1. Hash Tables
2. Linked lists
3. Red Black Trees

### Why these structures?
Due to the big amount of patient records entries, hash tables and red black trees can secure a small time complexity on performing specific queries.

### How to run..
There is a makefile that responds to the following commands:

  1. ```make``` -> compile the program and produce the corresponding object files and executable.

  2. ```make clean``` -> deletes the object files and executable that were created.

```./diseaseMonitor -p patientRecordsFile -h1 diseaseHashtableNumOfEntries -h2 countryHashtableNumOfEntries -b bucketSize```

*NOTE* : bucketSize must be >= 24

An example: h1 = 10, h2 = 10, bucketSize = 100.   

### Commands

* ```/globalDiseaseStats [date1 date2]```
  * Aplication will print for each virus, the number of cases recorded in the system. If
    given date1 date2 then the application will print for each virus, the number of cases they have
    recorded in the system within the time period [date1 ... date2].

* ```/diseaseFrequency virusName [country] date1 date2```
  * If no country argument is given, application will print the virusName number for the disease
    cases recorded in the system during [date1 ... date2].
  * If country argument is given, application will print for virusName disease, the number of cases in
    country recorded in space [date1 ... date2].

* ```/topk-Diseases k country [date1 date2]```

* ```/topk-Countries k disease [date1 date2]```

* ```/insertPatientRecord recordID patientFirstName patientLastName diseaseID entryDate [exitDate]```

* ```/recordPatientExit recordID exitDate```

* ```/numCurrentPatients [disease]```

* ```/exit```
