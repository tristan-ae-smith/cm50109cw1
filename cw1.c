#include <stdio.h>

typedef struct node {
	struct node *left, *right;
	int age;
	char fName[64]; //TODO validate assumptions
	char oName[64];
	int code;
} node;

//'comparator' is now a pointer to a function
// takes two node pointers and returns an int
// -1 for <; 0 for ==; 1 for >
typedef int (*comparator)(node*, node*);

typedef struct tree {
	node *root;
	comparator sortType;
} tree;

//add a node
tree* addNode(tree *t, node *n) {
	if (t->sortType == NULL) {
		printf("Node %d added to uninitialised tree; discarded.\n", n->code);
		return t;
	}

	//return root for future chaining
	return t;
}

int main(void)//TODO take args
{
	// do something with the args, maybe
	return 0;
}