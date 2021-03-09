#include "list.h"

struct list * new_list()
{
	struct list *list = malloc(sizeof(struct list));
	list->start = NULL; 
	list->end = NULL;	

	return list;
}

struct lnode * new_lnode(patientRecord * record)
{
	struct lnode * lnode = malloc(sizeof(struct lnode));
	lnode->patient = record;
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
	if(strcmp(list->start->patient->entryDate, key) == 0) //if key is in list->start
	{
		
		if(list->start == list->end) //if only one node in list
			list->end = NULL; 

		struct lnode *to_del = list->start;
		list->start = list->start->next; //update list->start
		deletePatientRecord(to_del->patient);
		free(to_del);

		return 1;
	}
	else
	{
		struct lnode *temp = list->start;

		while(temp->next != NULL)
		{
			if(strcmp(temp->next->patient->entryDate, key) == 0) 
			{
				if(list->end == temp->next) //if key in list->end update list->end
					list->end = temp;
					
				struct lnode *to_del = temp->next;
				temp->next = temp->next->next; //update temp->next
				deletePatientRecord(to_del->patient);
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
	{
		printf("List is empty (delete_list)\n");
		return;
	}
	else
	{ 
		struct lnode *prev = list->start;
		
		while(list->start != NULL)
		{
			list->start = list->start->next;
			deletePatientRecord(prev->patient);
			free(prev);
			prev = list->start;
		}
	}

	list->start = list->end = NULL;
	
}

void print_list(struct list * list)
{
	if(list->start == NULL)
	{
		printf("List is empty (print_list)\n");
		return;
	}
	else
	{
		struct lnode *temp = list->start;
		
		while(temp != NULL)
		{
			printRecord(temp->patient);
			temp = temp->next;
		}
	}
}

patientRecord * find_patient(struct list * list, int recID)
{
	if (list->start == NULL)
	{
//		printf("List is empty (find_patient)\n");
		return NULL;
	}
	else
	{ 
		struct lnode *temp = list->start;

		while(temp != NULL)
		{
			if(temp->patient->recordID == recID) 
				return temp->patient;
			else
				temp = temp->next;	
		}
	}

	return NULL;
}