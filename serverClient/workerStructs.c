#include "workerStructs.h"

///////////////////////////////////////////   list-lnode   ///////////////////////////////////////////

struct list * new_list()
{
	struct list *list = malloc(sizeof(struct list));
	list->start = NULL; 
	list->end = NULL;	

	return list;
}

struct lnode * new_lnode(char * date)
{
	struct lnode * lnode = malloc(sizeof(struct lnode));
	lnode->key = malloc (strlen(date) + 1);
	strcpy(lnode->key, date);
	lnode->next = NULL;
	
	return lnode;
}

void insert_lnode(struct list* list, struct lnode* lnode)
{
	if(list->start == NULL)
	{
		list->start = lnode;
		list->end = lnode;
	}
	else
	{
		list->end->next = lnode;
		list->end = lnode;
	}
}

int delete_key_list(struct list * list, char * key) //returns 1 if key was found and deleted in the list, else 0
{
	if(strcmp(list->start->key, key) == 0) //if key is in list->start
	{
		
		if(list->start == list->end) //if only one node in list
			list->end = NULL; 

		struct lnode *to_del = list->start;
		list->start = list->start->next; //update list->start
		free(to_del->key);
		free(to_del);

		return 1;
	}
	else
	{
		struct lnode *temp = list->start;

		while(temp->next != NULL)
		{
			if(strcmp(temp->next->key, key) == 0) 
			{
				if(list->end == temp->next) //if key in list->end update list->end
					list->end = temp;
					
				struct lnode *to_del = temp->next;
				temp->next = temp->next->next; //update temp->next
				free(to_del->key);
				free(to_del);	
				
				return 1;
			}
			else
				temp = temp->next;	
		}
	
		return 0;
	}
	
}

void delete_list(struct list * list)
{
	if (list->start == NULL)
		return;
	else
	{ 
		struct lnode *prev = list->start;
		
		while(list->start != NULL)
		{
			list->start = list->start->next;
			free(prev->key);
			free(prev);
			prev = list->start;
		}
	}

	list->start = list->end = NULL;
	
}

void print_list(struct list * list)
{
	if(list->start == NULL)
		return;
	else
	{
		struct lnode *temp = list->start;
		
		while(temp != NULL)
		{
			printf("%s\n", temp->key);
			temp = temp->next;
		}
	}
}

int key_exist(struct list * list, char * key)
{
    struct lnode * lnode = list->start;
    int exist = 0;

    while (lnode != NULL)
    {
        if (strcmp(lnode->key, key) == 0)
        {
            exist = 1;
            break;
        }
        else
            lnode = lnode->next;
    }

    return exist;
}

///////////////////////////////////////////   subdirectory-subdirectory_list   ///////////////////////////////////////////


struct subdirectory * new_subdirectory(char * path, int diseaseHashtableNumOfEntries)
{
    struct subdirectory * subdir = malloc(sizeof(struct subdirectory));
    
    subdir->diseaseHashtable = malloc(sizeof(struct bucket *) * diseaseHashtableNumOfEntries);
    for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
        subdir->diseaseHashtable[i] = NULL;

    subdir->next = NULL;

    subdir->path = malloc(strlen(path) + 1);
    strcpy(subdir->path, path);

    subdir->tree = new_RBTree();

    subdir->date_files = new_list();

    return subdir;
}

void insert_subdirectory(struct subdirectory_list * subdirectory_list, struct subdirectory * subdirectory)
{
	if(subdirectory_list->start == NULL)
	{
		subdirectory_list->start = subdirectory;
		subdirectory_list->end = subdirectory;
	}
	else
	{
		subdirectory_list->end->next = subdirectory;
		subdirectory_list->end = subdirectory;
	}
}

void insert_date_file(struct subdirectory * subdirectory, char * date)
{
    struct lnode * lnode = new_lnode(date);
    insert_lnode(subdirectory->date_files, lnode);
}


struct subdirectory_list * new_subdirectory_list()
{
    struct subdirectory_list * subdir_list = malloc(sizeof(struct subdirectory_list));
    subdir_list->start = NULL;
    subdir_list->end = NULL;

    return subdir_list;
}

void delete_subdirectory_list(struct subdirectory_list * subdirectory_list, int diseaseHashtableNumOfEntries)
{
	if (subdirectory_list->start == NULL)
		return;
	else
	{ 
		struct subdirectory *prev = subdirectory_list->start;
		
		while(subdirectory_list->start != NULL)
		{
			subdirectory_list->start = subdirectory_list->start->next;

            delete_list(prev->date_files);
            delete_hashtables(prev->diseaseHashtable, diseaseHashtableNumOfEntries);   
            destroyRBTree(prev->tree, prev->tree->root, 1); 
            free(prev->tree);
			free(prev->path);
            free(prev->date_files);
            free(prev);
			prev = subdirectory_list->start;
		}
	}

	subdirectory_list->start = subdirectory_list->end = NULL;    
}

void delete_hashtables(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries)
{

	struct bucket * prevBucket;
	struct bucket * tempBucket;
	struct bucketEntry * tempBucketEntry;

// Deleting diseaseHashtable
	for (int i = 0; i < diseaseHashtableNumOfEntries; ++i)
	{

		if (diseaseHashtable[i] != NULL)
		{
			tempBucket = diseaseHashtable[i];
			prevBucket = tempBucket;

			while (tempBucket != NULL)
			{
				tempBucket = tempBucket->next;

				for (int j = 0; j < prevBucket->numOfEntries; ++j)
				{
					tempBucketEntry = prevBucket->bytes + j * sizeof(struct bucketEntry); 

					free(tempBucketEntry->key);
					destroyRBTree(tempBucketEntry->tree, tempBucketEntry->tree->root, 0);
					free(tempBucketEntry->tree);
				}

				free(prevBucket->bytes);
				free(prevBucket);

				prevBucket = tempBucket;

			}
		}
	}

	free(diseaseHashtable);
}