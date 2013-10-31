#include <stdio.h>

#define ON_LENGTH 64
#define FN_LENGTH 64

typedef struct node {
	struct node *left, *right;
	int age;
	char fName[ON_LENGTH]; //TODO validate assumptions
	char oName[FN_LENGTH];
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

int byAge(node *x, node *y) {
	return x->age - y->age;
}

//add a node
tree* addNode(tree *t, node *n) {
	if (t->sortType == NULL) {
		printf("Node %d added to uninitialised tree; discarded.\n", n->code);
		return t;
	}

	//return tree for future chaining
	return t;
}

int main(int argc, char *argv[])
{
	// tree *people = malloc(sizeof(tree));
	// people->comparator = &byAge;

	int i;
	for (i = 1; i < argc; ++i)
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
			printf("%d %d %s %s", code, age, oName, oName);
		}
	}
	// do something with the args, maybe
	return 0;
}