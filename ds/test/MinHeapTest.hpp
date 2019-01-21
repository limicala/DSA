#include "MinHeap.h"

void test_heap_insert(){
	MinHeap heap;

	for(int i = 10; i >= 0; i--){
		heap.insert(i);
	}

	heap.display();
	printf("------\n");

	for(int i = 0; i < 4; i++){
		heap.remove();
		heap.display();
	}
	
}