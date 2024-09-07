#include <iostream>
#include <cstdlib>
#include <cstddef>

#define POOL_SIZE 1024 * 1024 // This is 1 MB

class MemoryAllocator {
private:
	char* memorypool;
	size_t poolsize;
	struct FreeBlock {
		size_t size;
		FreeBlock* next;
	};
	FreeBlock* freeList;

public:
	MemoryAllocator() {
		memorypool = (char*)malloc(POOL_SIZE);
		poolsize = POOL_SIZE;
		freeList = (FreeBlock*)memorypool;
		freeList->next = nullptr;

	}
	~MemoryAllocator() {
		free(memorypool);
	}

	void* allocate(size_t size) {
		FreeBlock* current = freeList;
		FreeBlock* previous = nullptr;

		size = (size + sizeof(FreeBlock) + 7) & ~7;


		while (current != nullptr && current->size < size) {
			previous = current;
			current = current->next;
		}
		if (current == nullptr) {
			return nullptr;
		}
		if (current->size > size + sizeof(FreeBlock)) {
			FreeBlock* newblock = (FreeBlock*)((char*)current + size);
			newblock->size = current->size - size;
			newblock->next = current->next;
			current->size = size;
			if (previous) {
				previous->next = newblock;
			}
			else {
				freeList = newblock;
			}
		}
		else {
			if (previous) {
				previous->next = current->next;
			}
			else {
				freeList = current->next;
			}
		}
	}
	void deallocate(void* ptr) {
		if (!ptr) return;

		FreeBlock* block = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));
		FreeBlock* current = freeList;
		FreeBlock* previous = nullptr;

		while (current != nullptr && current < block) {
			previous = current;
			current = current->next;
		}
		block->next = current;
		if (previous) {
			previous->next = block;
		}
		else {
			freeList = block;
		}

		if ((char*)block + block->size == (char*)block->next) {
			block->size += block->next->size;
			block->next = block->next->next;
		}
		if (previous && (char*)previous + previous->size == (char*)block) {
			previous->size += block->size;
			previous->next = block->next;
		}
	}
};


int main() {
	MemoryAllocator allocator;
	// WE ARE ALLOCATING 100 BYTES "EXAMPLE"
	void* ptr1 = allocator.allocate(100);
	std::cout << "Allocated 100 bytes at: " << ptr1 << std::endl;
	// WE ARE ALLOCATING 200 BYTES 
	void* ptr2 = allocator.allocate(200);
	std::cout << "We are allocating 200 bytes at: " <<ptr2 << std::endl;

	//WE ARE DEALLOCATING 100 BYTES: 
	allocator.deallocate(ptr1);
	std::cout << "Deallocated 100 bytes at: " << ptr1 << std::endl;
	//We are allocating again
	void* ptr3 = allocator.allocate(50);
	std::cout << "Reallocated 50 bytes at: " << ptr3 << std::endl;

	return 0;
}