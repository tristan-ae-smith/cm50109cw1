#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// assumed maximum lengths
#define ON_LENGTH 64
#define FN_LENGTH 64

// node stores references to left and right children, variables for data
typedef struct node {
	struct node *left, *right;
	int code;
	int age;
	char fName[FN_LENGTH]; //firstName
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

// operator to print the given node
void printer(node *p) {
	printf("%d\t%d\t%s\t%s", p->code, p->age, p->fName, p->oName);
	return;
}

void destructor(node *p) {
	if (NULL != p) {
		free(p);
	}
	return;
}

//'visitor' is now a pointer to a recursive function
// takes a node pointer and a function pointer
// visits all nodes beneath that node in appropriate order
// and performs the operator
typedef void (*visitor)(node*, operator);

// visitor to perform left-to-right traversal of the tree
void traverse(node *n, operator op) {
	if (n->left != NULL) {
		traverse(n->left, op);
	}
	op(n);
	if (n->right != NULL) {
		traverse(n->right, op);
	}
}

// visitor to perform right-to-left (reverse) traversal of the tree
void reverse(node *n, operator op) {
	if (n->right != NULL) {
		reverse(n->right, op);
	}
	op(n);
	if (n->left != NULL) {
		reverse(n->left, op);
	}
}

void postOrder(node *n, operator op) {
	if (n->right != NULL) {
		postOrder(n->right, op);
	}
	if (n->left != NULL) {
		postOrder(n->left, op);
	}
	op(n);
}

// tree stores a link to the first node and the selected comparator and visitor
typedef struct tree {
	node *root;
	comparator compare;
	visitor visitAll;
} tree;

// allocate memory for a new node, populate it, and return a pointer to it
node* makeNode(int code, int age, char *fName, char *oName) {
	node *n;
	if (NULL == (n = malloc(sizeof(node)))) {
		return NULL;
	}
	n->code = code;
	n->age = age;
	strcpy(n->fName, fName);
	strcpy(n->oName, oName);
	n->left = n->right = NULL;
	return n;
}

// add a node n to tree t using the tree's selected comparator
tree* addNode(tree *t, node *n) {
	if (NULL == n) {
		printf("Empty node added.\n");
		return t;
	}

	if (t->compare == NULL) {
		printf("Node %d added to uninitialised tree; discarded.\n", n->code);
		return t;
	}

	// special case if the tree is empty so far
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

// traverse the tree in post order, freeing each node and then the tree
int freeTree(tree *t) {
	if (NULL == t) {
		return 1;
	}
	postOrder(t->root, &destructor);
	free(t);
	return 0;
}

// print usage and flags information
void printHelp() {
	printf("treesort:\tread, merge and sort data records from files\n");
	printf("Usage:\t\ttreesort file1 <file2 file3 ..> -[flags]\n");
	printf("\t\tReads file1 and optional additional files\n");
	printf("\t\tSorts by age and prints to standard output\n");
	printf("Flags:\n");
	printf("\t-r\treverse output order\n");
	printf("\t-c\tsort by code\n");
	printf("\t-f\tsort by first name\n");
	printf("\t-l\tsort by last name\n");
	printf("\t-h\tprint this help\n");
	return;
}

// pesudorandomly generate test data that will sort in four different orders
void addTestData(tree *test) {

	int i, j;
	char first[10], last[10];

	for (i = j = 1; i < 25; ++i) {
		sprintf(first, "First%d", (25-i));
		sprintf(last, "Last%d\n", (100-j));
		j = i + ((j%2==0)? j/2 : j + 3);
		addNode(test, makeNode(i, j, first, last));
	}

	return;
}

// accept args, initialise tree, process flags, read files, output sorted data
int main(int argc, char *argv[])
{
	if (argc == 1) {
		printHelp();
		return 0;
	}

	// initialise a tree
	tree *people = malloc(sizeof(tree));
	people->compare = &byAge;
	people->visitAll = &traverse;

	// track if the -t flag was supplied
	char runTests = 0;

	int i;
	// scan for flags
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
						runTests = 1;
						break;
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

	// add test data after comparator is specified
	if (runTests) {
		addTestData(people);
	}

	// scan for filenames
	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] != '-')
		{
			FILE *file;
			// temporary variables to read into
			int code, age;
			char fName[FN_LENGTH], oName[ON_LENGTH];

			// attempt to open the file
			if ( (file = fopen(argv[i], "r")) == NULL) {
				printf("Can't open %s\n", argv[i]);
				continue;
			}

			// read each line to the end of the file
			while (!feof(file)) {
				// attempt to read the first three items
				if (fscanf(file, "%d %d %s ", &code, &age, fName) != 3) {
					fgets(oName, ON_LENGTH, file);
					continue;
				}
				// grab the remainder of the line
				fgets(oName, ON_LENGTH, file);

				// handle middle names, for full explanation see readme
				char *lastSpace = strrchr(oName, ' ');
				if (lastSpace != NULL) {
					*lastSpace = '\0';
					int fLen = strlen(fName);
					fName[fLen] = ' ';
					strcpy(fName + fLen + 1, oName);
					strcpy(oName, lastSpace + 1);
				}

				addNode(people, makeNode(code, age, fName, oName));
				// printf("%d, %d, %s, %s", code, age, fName, oName);
			}
		}
	}

	// if all files were unopenable or all data corrupt
	if (people->root == NULL) {
		printf("No valid data read\n");
		return 1;
	}

	// print a simple table header
	printf("Code\tAge\tFirst\tLast\n");

	// use the selected visitor to print each node in-order
	people->visitAll(people->root, &printer);

	freeTree(people);

	return 0;
}