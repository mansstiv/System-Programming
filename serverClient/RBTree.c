#include "RBTree.h"


// if key_type == 0 , key is entryDate type(char *)
// else if key_type == 1, key is recordID type(int) 
struct node * new_node(patientRecord * record, int key_type) 
{
	struct node * node = malloc(sizeof(struct node));
	
	if (key_type == 0)
	{
		node->key = malloc(strlen(record->entryDate) + 1);
		strcpy(node->key, record->entryDate);
	}
	else
	{
		node->key = malloc(sizeof(int));
		memmove(node->key, &record->recordID, sizeof(int));
	}
	
	node->patient = record; 
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->color = 'R';
	return node;
}


struct RBTree * new_RBTree() 
{
	struct RBTree * T = malloc(sizeof(struct RBTree));
	struct node * nil_node = malloc(sizeof(struct node));
	nil_node->left = NULL;
	nil_node->right = NULL;
	nil_node->parent = NULL;
	nil_node->color = 'B';
	nil_node->patient = NULL;
	T->NIL = nil_node;
	T->root = T->NIL;

	return T;
}

void LeftRotate(struct RBTree * T, struct node * x)
{	
	struct node * y = x->right; //set y
	x->right = y->left; //turn y's left subtree into x’s right subtree

	if(y->left != T->NIL) //update parent pointer of y's left
		y->left->parent = x;
	
	y->parent = x->parent; // link x's parent to y

	if(x->parent == T->NIL)  //x is root
		T->root = y;
	else if(x == x->parent->left)  //x is left child
		x->parent->left = y;
	else  //x is right child
		x->parent->right = y;
	
	y->left = x;  // put x on y's left
	x->parent = y;  //update parent pointer of x
}

void RightRotate(struct RBTree *T, struct node *y) 
{
	struct node * x = y->left;
	y->left = x->right;

	if(x->right != T->NIL) 
		x->right->parent = y;
	
	x->parent = y->parent;

	if(x->parent == T->NIL) 
		T->root = x;
	else if(y == y->parent->left) 
		y->parent->left = x;
	else 
		y->parent->right = x;
	
	x->right = y;
	y->parent = x;
}



void RBTinsertFixup(struct RBTree * T, struct node * z) 
{
	while (z->parent->color == 'R')
	{
		//CASE 1: Parent of z is in a left subtree
		if(z->parent == z->parent->parent->left)
		{
			struct node *y = z->parent->parent->right;
			// CASE 1.1: Aunt node y of z is red
			if(y->color == 'R') 
			{
				z->parent->color = 'B'; // After Colorflip:
				y->color = 'B';           //        RED
				z->parent->parent->color = 'R';   //  BLACK   BLACK
				z = z->parent->parent;
			}
			// CASE 1.2: Aunt node y of z is black 
			//There are 2 cases : RightRotate or LeftRightRotate
			//After rotatation:
			//        BLACK
			//      RED     RED
			else 
			{
				// CASE 1.2.1 : First RightRotate and then LeftRotate // z is a right child
				if (z == z->parent->right)  
				{
					z = z->parent;
					LeftRotate(T, z);
				}
			// CASE 1.2.2: RightRotate // z is a left child
				z->parent->color = 'B';
				z->parent->parent->color = 'R';
				RightRotate(T, z->parent->parent);
			}     
		}
		//CASE 2: Parent of z is in a right subtree
		//Similar to CASE 1 
		else
		{
			struct node * y = z->parent->parent->left;

			if(y->color == 'R')
			{
				z->parent->color = 'B';
				y->color = 'B';
				z->parent->parent->color = 'R';
				z = z->parent->parent;
			}
			else
			{
				if(z == z->parent->left)
				{
					z = z->parent;
					RightRotate(T, z);
				}
			
				z->parent->color = 'B';
				z->parent->parent->color = 'R';
				LeftRotate(T, z->parent->parent);
			}
		}
	}
	
	T->root->color = 'B';
}

// if key_type == 0 , key is entryDate type(char *)
// else if key_type == 1, key is recordID type(int) 
void RBTinsert(struct RBTree * T, struct node * z, int key_type) 
{  
	
	struct node * y = T->NIL;
	struct node * x = T->root;

	while(x != T->NIL)
	{
		y = x;

		// Key is entryDate
		if (key_type == 0) 
		{
			if(compareDates((char *)z->key, x->patient->entryDate) <= 0) 
				x = x->left;
			else
				x = x->right;   
		}
		// Key is recordID
		else
		{	
			if (*(int *)z->key <= x->patient->recordID)
				x = x->left;
			else
				x = x->right;	
		}		
	}
	
	z->parent = y;
	
	if (y == T->NIL)
		T->root = z;
	else
	{
		if (key_type == 0)
		{	
			
			if(compareDates((char *)z->key, y->patient->entryDate) <= 0) // duplicates in left subtree
				y->left = z;
			else
				y->right = z;
		}
		else
		{
			if (*(int *)z->key <= y->patient->recordID)
				y->left = z;
			else
				y->right = z;	
		}
	}

	z->right = T->NIL;
	z->left = T->NIL;

	RBTinsertFixup(T, z);
}

// TRANSPLANT
// node v is the brother of deleted node u
// v will replace u
// v is being updated
void transplant(struct RBTree * T, struct node * u, struct node * v) 
{
 	if(u->parent == T->NIL)
    	T->root = v;
  	else if(u == u->parent->left)
    	u->parent->left = v;
  	else
    	u->parent->right = v;
    
    v->parent = u->parent;
}

// returns node with minimum value in the subtree with root x
struct node * minimum(struct RBTree * T, struct node * x)
{
	while(x->left != T->NIL)
    		x = x->left;
  	return x;
}


void RBTdeleteFixUp(struct RBTree * T, struct node * x) 
{
	while(x != T->root && x->color == 'B') 
	{
		if(x == x->parent->left) // x is in a left subtree
		{
			struct node * w = x->parent->right; // w is the sibling of node x
			
			if(w->color == 'R') // Case 1: sibling node w of x is red
			{
				w->color = 'B';
				x->parent->color = 'R';
				LeftRotate(T, x->parent);
				w = x->parent->right;
			}

			if(w->left->color == 'B' && w->right->color == 'B') // Case 2: sibling node w of x is black
																//	children nodes of w are both black		 
			{
				w->color = 'R';
				x = x->parent;
			}
			else 
			{
				if(w->right->color == 'B') // Case 3: sibling node w of x is black - left child of w is red - right child of w is black
				{
					w->left->color = 'B';
					w->color = 'R';
					RightRotate(T, w);
					w = x->parent->right;
				}
				// Case 4: sibling node w of x is black - right child of w is red
				w->color = x->parent->color;
				x->parent->color = 'B';
				w->right->color = 'B';
				LeftRotate(T, x->parent);
				x = T->root;
			}
		}
		else // symmetrical situation
			// x is in a right subtree
		{
			struct node * w = x->parent->left;

			if(w->color == 'R') 
			{
				w->color = 'B';
				x->parent->color = 'R';
				RightRotate(T, x->parent);
				w = x->parent->left;
			}
			if(w->right->color == 'B' && w->left->color == 'B') 
			{
				w->color = 'R';
				x = x->parent;
			}
			else 
			{
				if(w->left->color == 'B') 
				{
					w->right->color = 'B';
					w->color = 'R';
					LeftRotate(T, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = 'B';
				w->left->color = 'B';
				RightRotate(T, x->parent);
				x = T->root;
			}
		}
	}

	x->color = 'B';
}

void RBTdelete(struct RBTree * T, struct node * z) 
{
	struct node * y = z;
	struct node * x;
	char originalColorY = y->color;

	if(z->left == T->NIL) // Case 1: Node to delete hasn't a left child
	{
		x = z->right;
		transplant(T, z, z->right);
	}
	else if(z->right == T->NIL) // Case 2: Node to delete hasn't a right child
	{
		x = z->left;
		transplant(T, z, z->left);
	}
	else // Case 3: Node to delete has 2 children
	{
		y = minimum(T, z->right);
		originalColorY = y->color;
		x = y->right;

		if(y->parent == z) 
			x->parent = y;
		else 
		{
			transplant(T, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		transplant(T, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	
	if(originalColorY == 'B')
		RBTdeleteFixUp(T, x);

}

void postorder(struct RBTree * T, struct node * node)
{
	if (T->NIL == NULL || T->root == T->NIL) 
	{
		return ;
	}

	if (node != T->NIL)
	{  
		postorder(T, node->left);
		postorder(T, node->right);
		// printf("%c\n", node->color);
		printRecord(node->patient);
		// printf("\n");
		
	}
}


int RBTdeleteKey(struct RBTree * T, void * key, int key_type, int flag_delete)
{
	int flag = 0;
	
	if(T->root == T->NIL)
		printf("Error: Tree is empty (deleteKeyRBT)\n");
	else
	{
		struct node * temp = T->root;
	
		while(temp != T->NIL)
		{
			if (key_type == 0)
			{
				if(compareDates((char *)temp->key, (char *)key) == 0)
				{
					flag = 1;
					break;
				}
				else if(compareDates((char *)temp->key, (char *)key) <= 0) 
					temp = temp->right;
				else //go left
					temp = temp->left;	
			}		
			else
			{
				if (*(int *)temp->key == *(int *)key)
				{
					flag = 1;
					break;
				}
				else if (*(int *)temp->key <= *(int *)key)
					temp = temp->right;
				else //go left
					temp = temp->left;				
			}
		}
		
		if(flag == 1) //key was found
		{
			RBTdelete(T, temp);
			if (flag_delete == 1 && temp->patient != NULL)
				deletePatientRecord(temp->patient);			
			free(temp->key);
			free(temp);
			return flag;
		}
		else
			return flag;
			
	}
	
}

// Traversing tree in postorder and deleting each node
void destroyRBTree(struct RBTree * T, struct node * node, int flag_delete)
{
	if (T->root == T->NIL || T->NIL == NULL) // Tree is created , but no nodes have been inserted
	{
		if (T->NIL != NULL)
			free(T->NIL);
		return;
	}

	if (node != T->NIL)
	{
		destroyRBTree(T, node->left, flag_delete);
		destroyRBTree(T, node->right, flag_delete);

		if (flag_delete == 1 && node->patient != NULL)
			deletePatientRecord(node->patient);
		
		free(node->key); 
		free(node);
	}

	if(node == T->root) // T->root was deleted in previous if , we have to free T->NIL now
	{
		free(T->NIL);
		T->NIL = NULL;
	}

}

patientRecord * RBTgetPatient(struct RBTree * T , void * key, int key_type)
{

	struct node* temp = T->root;
	
	while(temp != T->NIL)
	{	
		if (key_type == 0)
		{
			if(compareDates((char *)temp->key, (char *)key) == 0)
				return temp->patient;
			else if(compareDates((char *)temp->key, (char *)key) <= 0)
				temp = temp->right;
			else 
				temp = temp->left;	
		}
		else
		{
			if (*(int *)temp->key == *(int *)key)
				return temp->patient;
			else if (*(int *)temp->key <= *(int *)key)	
				temp = temp->right;
			else
				temp = temp->left; 	
		}
		
	}

	return NULL;
}

int get_number_of_patients(struct RBTree * T, struct node * node, char * date1, char * date2,  char discharged)
{	 
	int count_left, count_right;

	if (T->root == T->NIL || T->NIL == NULL || node == T->NIL) 
	{
		return 0;
	}

	count_left = get_number_of_patients(T, node->left, date1, date2, discharged);
	count_right = get_number_of_patients(T, node->right, date1, date2, discharged);

	if (discharged == 0)
		return ((compareDates(node->patient->entryDate, date1) >= 0 && compareDates(node->patient->entryDate, date2) <= 0) ? 1 : 0)
				+ count_left + count_right;
	else
	{
					// If patient has been discharged
		return (( (strcmp(node->patient->exitDate, "-") != 0) && 
					// If exitDate is between date parameters
			 	  (compareDates(node->patient->exitDate, date1) >= 0 && compareDates(node->patient->exitDate, date2) <= 0)) 
		 		  	// Count current node if previous expression is 1 else don't count + count_left + count_right
				   ? 1 : 0) + count_left + count_right;	
	
	}			
}

void initialize_listCases(struct listCases * list, struct RBTree * T, struct node * node, char * date1, char * date2, char * key)
{
	if (T->NIL == NULL || T->root == T->NIL) 
	{
		return ;
	}

	if (date1 == NULL)
	{
		if (node != T->NIL)
		{  
			initialize_listCases(list, T, node->left, NULL, NULL, key);
			initialize_listCases(list, T, node->right, NULL, NULL, key);	
			if (strcmp(key, "diseaseID") == 0)
				insert_case(list, node->patient->diseaseID, node->patient);
			else if (strcmp(key, "entryDate") == 0)
				insert_case(list, node->patient->entryDate, node->patient);
		}	
	}
	else
	{
		if (node != T->NIL)
		{
			initialize_listCases(list, T, node->left, date1, date2, key);
			initialize_listCases(list, T, node->right, date1, date2, key);	
			if (compareDates(node->patient->entryDate, date1) >= 0 && compareDates(node->patient->entryDate, date2) <= 0)
			{
				if (strcmp(key, "diseaseID") == 0)
					insert_case(list, node->patient->diseaseID, node->patient);
				else // Key is age
				{
					if (node->patient->age <= 20)
						insert_case(list, "0-20", node->patient);
					else if (node->patient->age > 20 && node->patient->age <= 40)
						insert_case(list, "21-40", node->patient);
					else if (node->patient->age > 40 && node->patient->age <= 60)
						insert_case(list, "41-60", node->patient);
					else		
						insert_case(list, "60+", node->patient);	
				}
			}
		}		
	}

}

