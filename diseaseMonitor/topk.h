#include "patientRecord.h"

struct listNode
{
	struct listNode * next;
	char * key; // A diseaseID or a country , based on topk question
	int cases;
};

struct listCases
{
	struct listNode * start;
};

struct tnode
{
	struct listNode * node;
	struct tnode * left;
	struct tnode * right;
	struct tnode * parent;
};

struct tree
{
	int numOfNodes;
	struct tnode * root;
};


//////////////////////////////////////////// TREE ////////////////////////////////////////////

struct tree * new_tree();

struct tnode * new_tnode (struct listNode * node);

void initialize_heap(struct tree * tree, struct listCases * list);

void insert_heap (struct tree * tree, struct tnode * tnode);

void pop(struct tree * tree, struct listCases * list);

// inspired by : 
// https://stackoverflow.com/questions/51506395/how-can-one-find-the-last-right-most-node-on-the-last-level-of-tree-which-is-a
struct tnode * get_parent_of_most_right_node(struct tree * tree, int index);

void swap(struct tnode * a, struct tnode * b);

void print_heap(struct tree * tree, struct tnode * tnode);	// For debugging purposes

void delete_heap(struct listCases * list, struct tree * tree, struct tnode * tnode);

//////////////////////////////////////////// LIST ////////////////////////////////////////////

struct listCases * new_listCases();

struct listNode * new_listNode(char * key);

void insert_case(struct listCases * list, char * key);

void delete_listNode(struct listCases * list, struct listNode * toDel);
