#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#define MAX_HEAP_SIZE 1024

class MinHeap {
public:
	MinHeap();

	void insert(const int e);
	int remove();
	void display();
private:
	int arr[MAX_HEAP_SIZE];
	int size;
};

#endif
