#include "RBTree.h"

struct node * new_node(patientRecord * record) 
{
	struct node * node = malloc(sizeof(struct node));
	node->key = malloc(strlen(record->entryDate) + 1);
	strcpy(node->key, record->entryDate);
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

void RBTinsert(struct RBTree * T, struct node * z) 
{  
	
	struct node * y = T->NIL;
	struct node * x = T->root;

	while(x != T->NIL)
	{
		y = x;
		if(compareDates(z->key, x->patient->entryDate) <= 0) 
			x = x->left;
		else
			x = x->right;   
	}
	
	z->parent = y;
	
	if(y == T->NIL)
		T->root = z;
	else if(compareDates(z->key, y->patient->entryDate) <= 0) // duplicates in left subtree
		y->left = z;
	else
		y->right = z;
	
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
		printf("Tree is empty (postorder)\n");
		return ;
	}

	if (node != T->NIL)
	{  
		postorder(T, node->left);
		postorder(T, node->right);
		printRecord(node->patient);
	}
}


int RBTdeleteKey(struct RBTree * T, char * key)
{
	int flag = 0;
	
	if(T->root == T->NIL)
		printf("Error: Tree is empty (deleteKeyRBT)\n");
	else
	{
		struct node * temp = T->root;
	
		while(temp != T->NIL)
		{
			if(compareDates(temp->key, key) == 0)
			{
				flag = 1;
				break;
			}
			else if(compareDates(temp->key, key) <= 0) 
				temp = temp->right;
			else //go left
				temp = temp->left;	
		}
		
		if(flag == 1) //key was found
		{
			RBTdelete(T, temp);
			return flag;
		}
		else
			return flag;
			
	}
	
}

// Traversing tree in postorder and deleting each node
void destroyRBTree(struct RBTree * T, struct node * node)
{
	if (T->root == T->NIL || T->NIL == NULL) // Tree is created , but no nodes have been inserted
	{
		printf("Tree is empty (destroyRBTree)\n");
		return;
	}

	if (node != T->NIL)
	{
		destroyRBTree(T, node->left);
		destroyRBTree(T, node->right);
//		printf("Deleting: ");
//		printRecord(node->patient);
		free(node->key); 
		free(node);
	}

	if(node == T->root) // T->root was deleted in previous if , we have to free T->NIL now
	{
		free(T->NIL);
		T->NIL = NULL;
	}

}

patientRecord * RBTgetPatient(struct RBTree * T , char * key)
{

	struct node* temp = T->root;
	
	while(temp != T->NIL)
	{
		if(compareDates(temp->key, key) == 0)
			return temp->patient;
		else if(compareDates(temp->key, key) <= 0)
			temp = temp->right;
		else //go left
			temp = temp->left;	
	}

	return NULL;
}

int get_number_of_patients(struct RBTree * T, struct node * node, char * date1, char * date2, char * country, char stillHospitalized)
{	 
	if (T->root == T->NIL || T->NIL == NULL) 
	{
		printf("Tree is empty\n"); 
		return 0;
	}

	// Case 1: no date1 , date2 parameters are given
		// country parameter is not taken into consideration
		// perhaps stillHospitalized parameter is given
	if (date1 == NULL && date2 == NULL)
	{	
		if (node != T->NIL)
		{
			if (stillHospitalized == 0)
			{
				get_number_of_patients(T, node->left, NULL, NULL, NULL, 0);
				get_number_of_patients(T, node->right, NULL, NULL, NULL, 0);
				count++;
			}
			else
			{
				get_number_of_patients(T, node->left, NULL, NULL, NULL, 1);
				get_number_of_patients(T, node->right, NULL, NULL, NULL, 1);				
				if (strcmp(node->patient->exitDate, "-") == 0)
					count++;
			}
		} 
	}
	// Case 2: date1 , date2 parameters are given
		// stillHospitalized parameter is not taken into consideration
		// perhaps country parameter is given
	else 
	{
		if (node != T->NIL)
		{
			if (country != NULL)
			{
				get_number_of_patients(T, node->left, date1, date2, country, 0);
				get_number_of_patients(T, node->right, date1, date2, country, 0);
				if (compareDates(node->patient->entryDate, date1) >= 0 && compareDates(node->patient->entryDate, date2) <= 0
					&& strcmp(node->patient->country, country) == 0)				
					count++;				
			}
			else
			{
				get_number_of_patients(T, node->left, date1, date2, NULL, 0);
				get_number_of_patients(T, node->right, date1, date2, NULL, 0);
				if (compareDates(node->patient->entryDate, date1) >= 0 && compareDates(node->patient->entryDate, date2) <= 0)
					count++;
			}
		}
	}
	
	return count;
		
}

void initialize_listCases(struct listCases * list, struct RBTree * T, struct node * node, char * date1, char * date2, char * key)
{
	if (T->NIL == NULL || T->root == T->NIL) 
	{
		printf("Tree is empty (postorder)\n");
		return ;
	}

	if (date1 == NULL)
	{
		if (node != T->NIL)
		{  
			initialize_listCases(list, T, node->left, NULL, NULL, key);
			initialize_listCases(list, T, node->right, NULL, NULL, key);	
			if (strcmp(key, "diseaseID") == 0)
				insert_case(list, node->patient->diseaseID);
			else
				insert_case(list, node->patient->country);
		}	
	}
	// date1 and date2 parameters are given
	else
	{
		if (node != T->NIL)
		{
			initialize_listCases(list, T, node->left, date1, date2, key);
			initialize_listCases(list, T, node->right, date1, date2, key);	
			if (compareDates(node->patient->entryDate, date1) >= 0 && compareDates(node->patient->entryDate, date2) <= 0)
			{
				if (strcmp(key, "diseaseID") == 0)
					insert_case(list, node->patient->diseaseID);
				else
					insert_case(list, node->patient->country);	
			}
		}		
	}

}