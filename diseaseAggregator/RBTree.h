#include "topk.h"

struct node {
	char color;  // for color property
	struct node * left, * right, * parent; // links for left, right children and parent
	patientRecord * patient;
	void * key; // entryDate or id
};

struct RBTree {
	struct node * root;
	struct node * NIL;
};

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

// char key_type parameter: if key_type == 0 key is entryDate type(char *)
// 							else if key_type == 1, key is recordID type(int)

struct node * new_node(patientRecord * record, int key_type);

struct RBTree * new_RBTree();

void LeftRotate(struct RBTree * T, struct node * x);

void RightRotate(struct RBTree * T, struct node * y);

void RBTinsertFixup(struct RBTree * T, struct node * z);

void RBTinsert(struct RBTree * T, struct node * z, int key_type);

void transplant(struct RBTree * T, struct node * u, struct node * v);

struct node * minimum(struct RBTree * T, struct node * x);

void RBTdeleteFixUp(struct RBTree * T, struct node * x);

void RBTdelete(struct RBTree * T, struct node * z);

void postorder(struct RBTree * T, struct node * node);   

// if flag_delete == 0 , do not free patientRecord 
// else free also patientRecord 
int RBTdeleteKey(struct RBTree * T, void * key, int key_type, int flag_delete);	

// Only this function is used to free memory allocated for RBTtree 
// Previous delete functions are not being used 
// if flag == 0 , do not free patientRecord 
// else free also patientRecord 
void destroyRBTree(struct RBTree * T, struct node * node, int flag_delete); 

patientRecord * RBTgetPatient(struct RBTree * T , void * key, int key_type);

// Initializes list used for topk questions
// This list holds a number of cases(num of cases of each disease of a country, or num of cases of each country of a disease) 
// If date1 and date2 != NULL, we reduce our research only in given time limits
// Key parameter for country or diseaseID
void initialize_listCases(struct listCases * list, struct RBTree * T, struct node * node, char * date1, char * date2, char * key);

// If discharged == 0 parameter is ignored else
// searches patients that have been discharged
int get_number_of_patients(struct RBTree * T, struct node * node, char * date1, char * date2,  char discharged); 
