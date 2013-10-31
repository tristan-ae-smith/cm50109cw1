#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define ON_LENGTH 64
#define FN_LENGTH 64

typedef struct node {
	struct node *left, *right;
	int code;
	int age;
	char fName[FN_LENGTH]; //firstName		//TODO validate assumptions
	char oName[ON_LENGTH]; //otherNames
} node;

//'comparator' is now a pointer to a function
// takes two node pointers and returns an int
// -ve for <; 0 for ==; +ve for >
typedef int (*comparator)(node*, node*);

int byAge(node *x, node *y) {
	return x->age - y->age;
}

int byCode(node *x, node *y) {
	return x->code - y->code;
}

int byFirst(node *x, node *y) {
	return strncmp(x->fName, y->fName, FN_LENGTH);
}

int byLast(node *x, node *y) {
	return strncmp(x->oName, y->oName, ON_LENGTH);
}


//'operator' is now a pointer to a function
// takes a node pointer and performs some operation
typedef void (*operator)(node*);

//operator to print the given node
void printer(node *p) {
	printf("%d\t%d\t%s\t%s", p->code, p->age, p->fName, p->oName);
	return;
}

//'visitor' is now a pointer to a recursive function
// takes a node pointer and a function pointer
// visits all nodes beneath that node in appropriate order
// and performs the operator
typedef void (*visitor)(node*, operator);

//visitor to perform left-to-right traversal of the tree
void traverse(node *n, operator op) {
	if (n->left != NULL) {
		traverse(n->left, op);
	}
	op(n);
	if (n->right != NULL) {
		traverse(n->right, op);
	}
}

//visitor to perform right-to-left (reverse) traversal of the tree
void reverse(node *n, operator op) {
	if (n->right != NULL) {
		reverse(n->right, op);
	}
	op(n);
	if (n->left != NULL) {
		reverse(n->left, op);
	}
}

typedef struct tree {
	node *root;
	comparator compare;
	visitor visitAll;
} tree;

node* makeNode(int code, int age, char *fName, char *oName) {
	node *n = malloc(sizeof(node));
	n->code = code;
	n->age = age;
	strcpy(n->fName, fName);
	strcpy(n->oName, oName);
	n->left = n->right = NULL;
	return n;
}

//add a node 		//TODO make this use a pointer-to-pointer
tree* addNode(tree *t, node *n) {
	if (t->compare == NULL) {
		printf("Node %d added to uninitialised tree; discarded.\n", n->code);
		return t;
	}

	if (t->root == NULL) {
		t->root = n;
		return t;
	}

	node *parent = t->root;
	node *next = parent;

	//iterate down into the tree until we find an empty pointer in the right place
	while (next != NULL) {
		parent = next;
		next = ( t->compare(parent, n) > 0 )? parent->left : parent->right;
	}
	
	if ( t->compare(parent, n) > 0 ) {
		parent->left = n;
	} else {
		parent->right = n;
	}

	//return tree for future chaining
	return t;
}

void printHelp() {
	printf("cw1:\tread, merge and sort data records from files\n");
	printf("Usage: cw1 file1 <file2 file3 ..> -[flags]\n");
	printf("\t Reads file1 and optional additional files\n");
	printf("\t Sorts by age and prints to standard output\n");
	printf("Flags:\n");
	printf("\t-r\treverse output order\n");
	printf("\t-c\tsort by code\n");
	printf("\t-f\tsort by first name\n");
	printf("\t-l\tsort by last name\n");
	printf("\t-h\tprint this help\n");
	return;
}

void runTests() {

}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printHelp();
		return 0;
	}

	tree *people = malloc(sizeof(tree));
	people->compare = &byAge;
	people->visitAll = &traverse;

	int i;
	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-') {
			// deal with flags
			int j;
			for (j = 1; argv[i][j] != '\0'; ++j) {
				char flag = tolower(argv[i][j]);

				switch (flag) {
					case 'h':
						printHelp();
						return 0;
					case 't':
						runTests();
						return 0;
					case 'r':
						people->visitAll = &reverse;
						break;
					case 'c':
						people->compare = &byCode;
						break;
					case 'a':
						people->compare = &byAge;
						break;
					case 'f':
						people->compare = &byFirst;
						break;
					case 'l':
						people->compare = &byLast;
						break;
					default: 
						printf("Unrecognised flag: %c\n", flag);
				}
			}
		}
	}

	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] != '-')
		{
			FILE *file;
			int code, age;
			char fName[FN_LENGTH], oName[ON_LENGTH];

			if ( (file = fopen(argv[i], "r")) == NULL) {
				printf("Can't open %s\n", argv[i]);
				return 1;
			}

			while (!feof(file)) {
				if (fscanf(file, "%d %d %s ", &code, &age, fName) != 3) {
					break;
				}
				fgets(oName, ON_LENGTH, file);
				addNode(people, makeNode(code, age, fName, oName));
				// printf("%d %d %s %s", code, age, fName, oName);
			}
		}
	}

	printf("Code\tAge\tFirst\tLast\n");
	people->visitAll(people->root, &printer);

	return 0;
}