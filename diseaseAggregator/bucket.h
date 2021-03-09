#include "RBTree.h"

struct bucketEntry
{
	char * key; // Name of disease or country
	struct RBTree * tree;
};

struct bucket
{
	struct bucket * next; // Pointer to next bucket
	void * bytes; // Including bucketEntries 
	int numOfEntries; // Number of bucket entries 
};

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

struct bucket * new_bucket(int bucketSize);

void initialize_bucket_entry(struct bucketEntry * entry, char * key); 

// flag: 0 for diseaseHashtable, 1 for countryHashtable
// Inserts patient to the correct bucket and bucketEntry
void insert_patient_to_bucket(struct bucket * bucket, patientRecord * patient, int bucketSize); 

