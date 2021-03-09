#include "helpFunc.h"

// Returns -1 if wrong input, else > 0
void globalDiseaseStats(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries); 

void diseaseFrequency(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries);

void insertPatientRecord(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries,
						struct bucket ** countryHashtable, int countryHashtableNumOfEntries, struct list * list, int bucketSize);

void recordPatientExit(char * buffer, struct list * list);

void numCurrentPatients(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries);

void topkDiseases(char * buffer, struct bucket ** countryHashtable, int countryHashtableNumOfEntries);

void topkCountries(char * buffer, struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries);