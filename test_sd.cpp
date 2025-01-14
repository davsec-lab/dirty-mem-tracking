#include "sd.h"
#include <iostream>

void add_memory_region(VMARegion* vma) {
	TargetAppMemory* targetAppMemory = TargetAppMemory::getTargetAppMemory();
	targetAppMemory->addVMARegion(vma);
}

int main(void) {
	char* test_region = (char*) malloc(2*0x1000);
	VMARegion* vma = new VMARegion((Address)test_region,
			(Address)test_region + 2*0x1000);
	add_memory_region(vma);

	// Round 1
	clear_and_start_recording();
	// Write
	char* pointer = test_region;
	for (int i = 0; i < 100; i++) {
		*pointer++ = 'a';
	}

	std::vector<Address> dirtyPageVec;

	// See if it was dirty
	getDirtyPages(dirtyPageVec);
	std::cout << "Dumping dirty pages\n";
	for (int i = 0; i < dirtyPageVec.size(); i++) {
		std::cout << dirtyPageVec[i] << "\n";
	}

	
	// Round 2
	clear_and_start_recording();
	char* another_buffer = (char*) malloc(2*0x1000);
	pointer = another_buffer;
	for (int i = 0; i < 100; i++) {
		*pointer++ = 'b';
	}
	// See if it was dirty
	getDirtyPages(dirtyPageVec);
	std::cout << "Dumping dirty pages\n";
	for (int i = 0; i < dirtyPageVec.size(); i++) {
		std::cout << dirtyPageVec[i] << "\n";
	}


	return 0;
}


