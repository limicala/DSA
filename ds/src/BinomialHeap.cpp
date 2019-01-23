#include "BinomialHeap.h"

#include <stdio.h>
#include <stdlib.h>

BinomialHeap::BinomialHeap()
	: root(NULL) {}

BinomialHeap::BinomialHeap(const int e)
	: root(NULL) {
	
}

void BinomialHeap::adjust(){

}

void BinomialHeap::merge(BinomialNode* node){
	

	adjust();
}

void BinomialHeap::insert(const int e){
	BinomialNode* node = (BinomialNode*) malloc (sizeof(BinomialNode));

	node->element = e;
	node->degree = 0;
	node->child = NULL;
	node->parent = NULL;
	node->next = NULL;
	
	merge(node);
}