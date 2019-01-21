#ifndef BINARY_TREE_H
#define BINARY_TREE_H
struct BinaryNode {
	int element;
	BinaryNode* left;
	BinaryNode* right;
};

class BinaryTree {

public:
	BinaryTree();

	void insert(const int e);
	void remove(const int e);
	
	BinaryNode* findMin(BinaryNode* seek);

	void display();
	
private:
	

	BinaryNode* root;

	void insert(const int e, BinaryNode* & seek);
	void remove(const int e, BinaryNode* & seek);

	void display(const BinaryNode* start);
};
#endif