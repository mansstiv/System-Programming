#include "bucket.h"

struct bucket * new_bucket(int bucketSize)
{
	struct bucket * bucket1 = malloc(sizeof(struct bucket)); 
	bucket1->bytes = malloc(bucketSize);
	memset(bucket1->bytes, 0, bucketSize);
	bucket1->next = NULL;
	bucket1->numOfEntries = 0;

	return bucket1;
}

void initialize_bucket_entry(struct bucketEntry * entry, char * key)
{	
	entry->key = malloc(strlen(key) + 1);
	strcpy(entry->key, key);
	entry->tree = new_RBTree();
}

void insert_patient_to_bucket(struct bucket * bucket, patientRecord * patient, int bucketSize)
{
	// tempBucket points to the start of the /list of buckets/ of the /correct position/ of /our hasharray/ 
	struct bucket * tempBucket = NULL;
	struct bucketEntry tempBucketEntry;
	struct node * tempRBTnode = NULL;
	char found = 0; // found = 0 -> didn't found appropriate bucketEntry for patient, else 1   
	int maxEntries;

	tempBucket = bucket;
	maxEntries = (bucketSize - sizeof(struct bucket *)) / sizeof(struct bucketEntry);

	// Find right bucket to insert patient
	while(1)
	{
		if (tempBucket->numOfEntries == 0)
			break;

		// Check if a bucketEntry with same key as patient's key already exists 
		for (int i = 0; i < tempBucket->numOfEntries; ++i)
		{
			memmove(&tempBucketEntry, tempBucket->bytes + i * sizeof(struct bucketEntry), sizeof(struct bucketEntry));

			// Found appropriate bucketEntry so insert to tree
			if (strcmp(tempBucketEntry.key, patient->diseaseID) == 0) 
			{
				tempRBTnode = new_node(patient, 0); 
				RBTinsert(tempBucketEntry.tree, tempRBTnode, 0);
				found = 1;
				break;
			}
		}	

		if (found == 1)
			break;
		else
		{
			if (tempBucket->next == NULL)
				break;
			else 
				tempBucket = tempBucket->next;
		}
	}

	// Didn't find appropriate bucketEntry so create a new one
	// tempBucket points now points to the last bucket of our list of buckets
	if (found == 0)
	{
		// If no free space in bucket
		if (tempBucket->numOfEntries == maxEntries)
		{
			tempBucket->next = new_bucket(bucketSize);
			tempBucket = tempBucket->next;
		}

		initialize_bucket_entry(&tempBucketEntry, patient->diseaseID);
		tempRBTnode = new_node(patient, 0);
		RBTinsert(tempBucketEntry.tree, tempRBTnode, 0);
		tempBucket->numOfEntries++;
		memmove(tempBucket->bytes + (tempBucket->numOfEntries - 1) * sizeof(struct bucketEntry), &tempBucketEntry, sizeof(struct bucketEntry));

	}
}

