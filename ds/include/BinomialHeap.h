#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

/* 
	https://www.cnblogs.com/skywang12345/p/3655900.html
*/

struct BinomialNode {
	int element;

	int degree;
	BinomialNode* parent;
	BinomialNode* child;
	BinomialNode* next;
};

class BinomialHeap {

public:
	BinomialHeap();
	BinomialHeap(const int e);

	void merge(BinomialNode* node);

	void insert(const int e);

	void display();
	
private:
	void adjust();
	
private:
	BinomialNode* root;
};

#endif