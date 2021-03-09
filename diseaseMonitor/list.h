#include "RBTree.h"

struct lnode {
	struct lnode * next;
	patientRecord * patient;;
};

struct list {
	struct lnode * start;
	struct lnode * end;
};

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

struct list * new_list();

struct lnode * new_lnode(patientRecord * record);

void insert_lnode(struct list * list, struct lnode * lnode);

int delete_key_list(struct list * list, char * key);

void delete_list(struct list * list);

void print_list(struct list * list);

// Returns pointer to patient or NULL if doesn't exist
patientRecord * find_patient(struct list * list, int recID);
