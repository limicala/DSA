#include "BinaryTree.h"

#include <stdio.h>
#include <stdlib.h>

BinaryTree::BinaryTree()
	: root(NULL) {}

void BinaryTree::insert(const int e){
	insert(e, root);
}

void BinaryTree::insert(const int e, BinaryNode* & seek){
	if(NULL == seek){
		seek = (BinaryNode*) malloc (sizeof(BinaryNode));
		seek->element = e;

		seek->left  = NULL;
		seek->right = NULL;

		return;
	}else if(e < seek->element){
		insert(e, seek->left);
	}else{
		insert(e, seek->right);
	}
}

void BinaryTree::remove(const int e){
	remove(e, root);
}

void BinaryTree::remove(const int e, BinaryNode* & seek){
	if(NULL == seek){
		return;
	}
	if(e == seek->element){
		if(NULL != seek->left && NULL != seek->right){
			BinaryNode* minNode = findMin(seek->right);
			seek->element = minNode->element;
			remove(minNode->element, seek->right);
		}else{
			BinaryNode* old = seek;
			seek = (seek->left) ? seek->left : seek->right;
			free(old);
		}
		return;
	}

	if(e < seek->element){
		remove(e, seek->left);
	}else{
		remove(e, seek->right);
	}
}


BinaryNode* BinaryTree::findMin(BinaryNode* seek){
	if(NULL == seek->left){
		return seek;
	}
	return findMin(seek->left);
}
void BinaryTree::display(){
	display(root);
	printf("\n");
}

void BinaryTree::display(const BinaryNode* start){
	if(NULL == start){
		return;
	}
	if(start->left != NULL){
		display(start->left);
	}

	printf("[%p(%d)] [%p(%d)] [%p(%d)] \n", start, start->element, start->left, start->left!=NULL? start->left->element:-1, start->right, start->right!=NULL?start->right->element:-1);
	if(start->right != NULL){
		display(start->right);
	}
}