#include "topk.h"


//////////////////////////////////////////// LIST ////////////////////////////////////////////

struct listCases * new_listCases()
{
	struct listCases * list = malloc(sizeof(struct listCases));
	list->start = NULL;
	list->total_cases = 0;

	return list;
}

struct listNode * new_listNode(char * key)
{
	struct listNode * node = malloc(sizeof(struct listNode));
	node->next = NULL;
	node->cases = 1;
	node->key = malloc(strlen(key) + 1);
	strcpy(node->key, key);
	for (int i = 0; i < 4; ++i)
		node->age_cases[i] = 0;

	return node; 
}

void insert_case(struct listCases * list, char * key, patientRecord * patient)
{
	struct listNode * temp = list->start;
	struct listNode * prev;

	if (list->start == NULL)
	{
		list->start = new_listNode(key);
		insert_age_case(list->start, patient);
		list->total_cases++;

		return;
	}

	while (temp != NULL)
	{	
		if (strcmp(temp->key, key) == 0)
		{
			temp->cases++;
			list->total_cases++;
			insert_age_case(temp, patient);

			return;
		}	

		prev = temp;
		temp = temp->next;
	}

	prev->next = new_listNode(key);
	insert_age_case(prev->next, patient);
	list->total_cases++;
}

void insert_age_case(struct listNode * lnode, patientRecord * patient)
{
	if (patient->age <= 20)
		lnode->age_cases[0]++;
	else if (patient->age > 20 && patient->age <= 40)
		lnode->age_cases[1]++;
	else if (patient->age > 40 && patient->age <= 60)
		lnode->age_cases[2]++;
	else		
		lnode->age_cases[3]++;
}


void delete_listNode(struct listCases * list, struct listNode * toDel)
{
	struct listNode * temp = list->start; 
	struct listNode * prev;

	// Empty list
	if (list->start == NULL)
		return;

	// Edge case : toDel is 1st item of list
	if (toDel == list->start)
	{
		list->start = toDel->next;
		free(toDel->key);
		free(toDel);

		return;
	}

	while (temp != toDel)
	{
		prev = temp;
		temp = temp->next;
	}

	prev->next = toDel->next;
	free(toDel->key);
	free(toDel);
}

void delete_listCases(struct listCases * list)
{
	if (list->start == NULL)
	{
		return;
	}
	else
	{ 
		struct listNode *prev = list->start;
		
		while(list->start != NULL)
		{
			list->start = list->start->next;
			free(prev->key);
			free(prev);
			prev = list->start;
		}
	}

	list->start = NULL;	

}

void print_listCases(struct listCases * list)
{
	struct listNode * temp = list->start;

	if (list->start == NULL)
		return;

	while (temp != NULL)
	{
		printf("%d ",temp->cases);
		temp = temp->next;
	}

	printf("\n");
}

void print_statistic(struct listCases * list, struct listNode * node)
{
	float stat = 100 * ((float)node->cases/list->total_cases);
    printf("%1.f%%\n", stat);
}

//////////////////////////////////////////// TREE ////////////////////////////////////////////

struct tree * new_tree()
{
	struct tree * tree = malloc (sizeof(struct tree));
	tree->numOfNodes = 0;
	tree->root = NULL;

	return tree;
}

struct tnode * new_tnode(struct listNode * node)
{
	struct tnode * tnode = malloc(sizeof(struct tnode));
	tnode->node = node;
	tnode->left = NULL;
	tnode->right = NULL;
	tnode->parent = NULL;

	return tnode;
}

void initialize_heap(struct tree * tree, struct listCases * list)
{
	struct listNode * tempListNode = list->start;
	struct tnode * tempTreeNode;	

	if (list->start == NULL)
		return;

	while(tempListNode != NULL)
	{
		tempTreeNode = new_tnode(tempListNode);
		insert_heap(tree, tempTreeNode);
		tempListNode = tempListNode->next;
	}
}

void insert_heap (struct tree * tree, struct tnode * tnode)
{
	tree->numOfNodes++;

	if (tree->root == NULL)	
	{	
		tree->root = tnode;
		return;
	}	

	struct tnode * parent = get_parent_of_most_right_node(tree, tree->numOfNodes);
	tnode->parent = parent;

	if (parent->left == NULL)
		parent->left = tnode;
	else
		parent->right = tnode;	

	struct tnode * tempNode = tnode;

	while(tempNode->parent != NULL)
	{
		if (tempNode->parent->node->cases < tempNode->node->cases)
		{
			swap(tempNode->parent, tempNode);
			tempNode = tempNode->parent;
		}
		else
			break;
	}
	
}

void pop(struct tree * tree, struct listCases * list, char ** message)
{
	struct tnode  * tempNode;	
	struct tnode * parent;
	int tempVal;

	if (tree->numOfNodes == 0)
		return;
	
	if (tree->numOfNodes == 1)
	{	
		tree->numOfNodes--;
		*message = malloc(strlen(tree->root->node->key) + strlen(":  ") + 6);
		sprintf(*message, "%s: %1.f%%\n", tree->root->node->key, 100 * ((float)tree->root->node->cases/list->total_cases));
		delete_listNode(list, tree->root->node);
		free(tree->root);
		tree->root = NULL;
		return;
	}

	*message = malloc(strlen(tree->root->node->key) + strlen(":  ") + 6);
	sprintf(*message, "%s: %1.f%%\n", tree->root->node->key, 100 * ((float)tree->root->node->cases/list->total_cases));

	parent = get_parent_of_most_right_node(tree, tree->numOfNodes);

	if (parent->right == NULL)
	{
		swap(tree->root, parent->left);
		delete_listNode(list, parent->left->node);
		free(parent->left);
		parent->left = NULL;
	}
	else
	{
		swap(tree->root, parent->right);
		delete_listNode(list, parent->right->node);
		free(parent->right);
		parent->right = NULL;
	}
	
	tempNode = tree->root;
	// We have to maintain heap property
	while (1)
	{
		// Case 1: 2 children
		// Swap with the largest child node if its necessary
		if (tempNode->left != NULL && tempNode->right != NULL)
		{
			if (tempNode->left->node->cases >= tempNode->right->node->cases)
			{	
				if (tempNode->left->node->cases > tempNode->node->cases)
				{	
					swap(tempNode, tempNode->left);
					tempNode = tempNode->left;
				}
				else
					break;
			}
			else
			{
				if (tempNode->right->node->cases > tempNode->node->cases)
				{
					swap(tempNode, tempNode->right);
					tempNode = tempNode->right;		
				}		
				else
					break;
			}
		}
		// Case 2: Only left child
		else if (tempNode->left != NULL && tempNode->right == NULL)
		{
			if (tempNode->left->node->cases > tempNode->node->cases)
			{	
				swap(tempNode, tempNode->left);
				tempNode = tempNode->left;
			}
			else
				break;
		}
		else 
			break;
	}

	tree->numOfNodes--;

}

struct tnode * get_parent_of_most_right_node(struct tree * tree, int index)
{
	struct tnode * tempNode = tree->root;
	int tempVal = index;

	while (tempVal & (tempVal - 1))
		tempVal &= tempVal - 1;
	
	tempVal >>= 1;

    while (tempVal) 
    {
        if (index & tempVal) 
        {
            if (tempNode->right)
                tempNode = tempNode->right;
            else
                return tempNode;
        } 
        else 
        {
            if (tempNode->left)
                tempNode = tempNode->left;
            else
                return tempNode; 
        }

        tempVal >>= 1;
    }	

    return tempNode->parent;
}

void swap(struct tnode * a, struct tnode * b)
{

	struct listNode * temp = a->node;
	a->node= b->node;
	b->node = temp;
}

void print_heap(struct tree * tree, struct tnode * tnode)
{
	if (tree->root == NULL)
		return;
	else
	{
		if (tnode != NULL)
		{
			print_heap(tree, tnode->left);
			print_heap(tree, tnode->right);
			printf("%s with %d cases\n", tnode->node->key, tnode->node->cases);
		}
	}
}

void delete_heap(struct listCases * list, struct tree * tree, struct tnode * tnode)
{
	if (tree->root == NULL)
		return;
	else
	{
		if (tnode != NULL)
		{
			delete_heap(list, tree, tnode->left);
			delete_heap(list, tree, tnode->right);
			delete_listNode(list, tnode->node);
			free(tnode);
		}
	}	
}

/*

int main ()
{	

	struct listCases * list1 = new_listCases();
	struct tree * heap = new_tree();
	insert_case(list1, "A");
	insert_case(list1, "A");
	insert_case(list1, "B");
	insert_case(list1, "c");
	insert_case(list1, "c");
	insert_case(list1, "B");
	insert_case(list1, "A");
	initialize_heap(heap, list1);
	print_heap(heap, heap->root);

	int x = 10;
	int num = heap->numOfNodes;

	if (x > num)
		x = num;

	for (int i = 0; i < x; ++i)
	{
		printf("|%d|\n", pop(heap, list1));
		print_heap(heap, heap->root);
		printf("\n");
	}

	free(heap);
	free(list1);
	return 1;


}

*/