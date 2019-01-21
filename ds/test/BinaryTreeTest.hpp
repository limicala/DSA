#include "BinaryTree.h"
#include <stdio.h>
void test(){
	BinaryTree tree;

	int a[] = {1,4,2,6,8,7};

	for(int i = 0; i < sizeof(a)/sizeof(a[0]); i++){
		tree.insert(a[i]);
	}
	tree.display();
	printf("-------\n");

	int remove[3] = {4,7,1};
	for(int i = 0; i < sizeof(remove)/sizeof(remove[0]); i++){
		tree.remove(remove[i]);
		tree.display();
	}
}