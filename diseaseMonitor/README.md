# Data Structures

# Disease Monitor
<p><b>Purpose</b> of this project is to implement a program, which based on an input file with patient records, can answer different questions about disease cases.</p>
<p>Tests <b>knowledge</b> on <b>data structures</b> and <b>time complexity</b>.</p>

## Input
A [patientRecordsFile](diseaseMonitor/small.txt) with patient records entries. A record could look like ```889 Mary Smith COVID-2019 China 25-1-2019 27-1-2019```.

## Structures
1. Hash Tables
2. Linked lists
3. Red Black Trees

## Why these structures?
Due to the big amount of patient records entries, hash tables and red black trees can secure a small time complexity on performing specific queries.

## How to run..
There is a makefile that responds to the following commands:

  1. ```make``` -> compile the program and produce the corresponding object files and executable.

  2. ```make clean``` -> deletes the object files and executable that were created.

```./diseaseMonitor -p patientRecordsFile -h1 diseaseHashtableNumOfEntries -h2 countryHashtableNumOfEntries -b bucketSize```

*NOTE* : bucketSize must be >= 24

* For example run: ```./diseaseMonitor -p small.txt -h1 10 -h2 10 -b 100```

## Commands

* ```/globalDiseaseStats [date1 date2]```
  * Print for each virus, the number of cases recorded in the system. If
    date1 date2 are given, print for each virus, the number of cases recorded within the time period [date1 ... date2].

* ```/diseaseFrequency virusName [country] date1 date2```
  * If no country argument is given, print for virusName disease, the number of cases
    cases recorded during time period [date1 ... date2].
  * If country argument is given, print for virusName disease, the number of cases in
    country recorded during time period [date1 ... date2].

* ```/topk-Diseases k country [date1 date2]```
  * Print for the country given, top k diseases with most cases recorded
    during time period [date1 ... date2]. 

* ```/topk-Countries k disease [date1 date2]```
  * For the disease given, print top k countries with the most cases in time period [date1 ... date2]. .

* ```/insertPatientRecord recordID patientFirstName patientLastName diseaseID entryDate [exitDate]```
  * Insert a new patient record, with the characteristics given (exitDate parameter is
    optional).

* ```/recordPatientExit recordID exitDate```
  * Add exit date to patient record with recordID id.

* ```/numCurrentPatients [disease]```
  * If disease argument is given, print the number of patients, who are still being treated with
    this disease. If no argument is given, do the same, but for every disease.

* ```/exit```
  * Exit program.

