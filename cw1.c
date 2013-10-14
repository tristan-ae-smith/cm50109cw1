#include <stdio.h>

typedef struct node {
	node *left, *right;
	int age;
	char fName[64]; //TODO validate assumptions
	char oName[64];
	int code;
};

typedef struct tree {
	node *root;
	node* (*comparator)(node *a, node *b) = NULL;
};

//add a node
node* addNode(tree *root, node* newNode) {
	if (root->comparator == NULL) {
		printf("Node %d added to uninitialised tree; discarded.\n", newNode.code);
		return root;
	}

	//return root for future chaining
	return root;
}

/* This is a
	block comment */
int main(void)
{
	// do something interesting
	printf("hello world\n");
	return 0;
}