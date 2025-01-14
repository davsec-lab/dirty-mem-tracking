#include "sd.h"
#include <sys/mman.h>
#include <stdio.h>
#include <iostream>

void add_memory_region(VMARegion* vma) {
	TargetAppMemory* targetAppMemory = TargetAppMemory::getTargetAppMemory();
	targetAppMemory->addVMARegion(vma);
}

#define NUMPAGES 4

int main(void) {
	char* test_region = (char*) mmap(nullptr, NUMPAGES*0x1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	VMARegion* vma = new VMARegion((Address)test_region,
			(Address)test_region + NUMPAGES*0x1000);
	add_memory_region(vma);

	// Round 1
	clear_and_start_recording();
	// Write
	volatile char* pointer = test_region;
	printf("Writing to pointer at %p\n", pointer);
	for (int i = 0; i < 100; i++) {
		*pointer++ = 'a';
	}

	std::vector<Address> dirtyPageVec;

	// See if it was dirty
	getDirtyPages(dirtyPageVec);
	std::cout << "Dumping dirty pages\n";
	for (int i = 0; i < dirtyPageVec.size(); i++) {
		std::cout << std::hex << dirtyPageVec[i] << "\n";
	}

	
	// Round 2
	dirtyPageVec.clear();

	clear_and_start_recording();

	char* another_buffer = (char*) mmap(nullptr, NUMPAGES*0x1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	pointer = another_buffer;
	printf("Writing to pointer at %p\n", pointer);

	for (int i = 0; i < 100; i++) {
		*pointer++ = 'b';
	}
	// See if it was dirty
	getDirtyPages(dirtyPageVec);
	std::cout << "Dumping dirty pages\n";
	for (int i = 0; i < dirtyPageVec.size(); i++) {
		std::cout << std::hex << dirtyPageVec[i] << "\n";
	}

	munmap(test_region, 2*0x1000);
	//munmap(another_buffer, 2*0x1000);
	return 0;
}


