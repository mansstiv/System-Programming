#include "topk.h"

struct node {
	char color;  // for color property
	struct node * left, * right, * parent; // links for left, right children and parent
	patientRecord * patient;
	char * key; // entryDate
};

struct RBTree {
	struct node * root;
	struct node * NIL;
};

//////////////////////////////////////////// FUNCTIONS ////////////////////////////////////////////

struct node * new_node(patientRecord * record);

struct RBTree * new_RBTree();

void LeftRotate(struct RBTree * T, struct node * x);

void RightRotate(struct RBTree * T, struct node * y);

void RBTinsertFixup(struct RBTree * T, struct node * z);

void RBTinsert(struct RBTree * T, struct node * z);

void transplant(struct RBTree * T, struct node * u, struct node * v);

struct node * minimum(struct RBTree * T, struct node * x);

void RBTdeleteFixUp(struct RBTree * T, struct node * x);

void RBTdelete(struct RBTree * T, struct node * z);

void postorder(struct RBTree * T, struct node * node);   

int RBTdeleteKey(struct RBTree * T, char * key);		

// Only this function is used to free memory allocated for RBTtree 
// Previous delete functions are not being used 
void destroyRBTree(struct RBTree * T, struct node * node); 

patientRecord * RBTgetPatient(struct RBTree * T , char * key);

// Initializes list used for topk questions
// This list holds a number of cases(num of cases of each disease of a country, or num of cases of each country of a disease) 
// If date1 and date2 != NULL, we reduce our research only in given time limits
// Key parameter for country or diseaseID
void initialize_listCases(struct listCases * list, struct RBTree * T, struct node * node, char * date1, char * date2, char * key);

// If entryDate1 and entryDate2 == NULL count all nodes
	// if stillHospitalized == 1 take into consideration only patients that are still hospitalized 
// else count nodes with date1 <= entryDate <= date2
	// If country is not NULL, then patients taken into consideration must be from this country
int get_number_of_patients(struct RBTree * T, struct node * node, char * date1, char * date2, char * country, char stillHospitalized); 