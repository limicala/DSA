#include "MinHeap.h"

#include <stdio.h>

// wont use index 0
MinHeap::MinHeap()
	: size(0) {}

void MinHeap::insert(const int e){
	int index = ++size;
	int parent_index = size / 2;

	arr[index] = e;

	while(parent_index > 0 && arr[index] < arr[parent_index]){
		int t = arr[index];
		arr[index] = arr[parent_index];
		arr[parent_index] = t;

		index = parent_index;
		parent_index /= 2;
	}
}

int MinHeap::remove(){
	int tmp = arr[1];

	arr[1] = arr[size];
	size -= 1;

	int index = 1;
	int left_child = 2 * index;
	int right_child = left_child + 1;

	while(left_child <= size && right_child <= size && (arr[index] > arr[left_child] || arr[index] > arr[right_child]) ){
		int t = arr[index];
		int k = (arr[left_child] < arr[right_child])? left_child : right_child;
		arr[index] = arr[k];
		arr[k] = t;

		index = k;
		left_child = 2 * index;
	    right_child = left_child + 1;
	}

	return tmp;
}

void MinHeap::display(){
	for(int i = 1; i <= size; i++){
		printf("[%d] ", arr[i]);
	}
	printf("\n");
}