#include "bucket.h"

unsigned int hash(char *str, unsigned int HASHSIZE);

void insert_to_structs(char * patientRecordsFile, struct list * list, int bucketSize, struct bucket ** diseaseHashtable, 
						int diseaseHashtableNumOfEntries, struct bucket ** countryHashtable, int countryHashtableNumOfEntries);

void uber_print(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries); // For debugging purposes

void delete_hashtables(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries, 
						struct bucket ** countryHashtable, int countryHashtableNumOfEntries);

int get_case_num(char * buffer);



