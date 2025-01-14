#include <iostream>
#include "memory.h"


#define PAGE_SIZE 4096

int readSoftDirty(int fd, unsigned long start_address, unsigned long end_address, std::vector<Address>& dirtyPageVec) {
	int len = (end_address - start_address) / PAGE_SIZE; // read these pages
	unsigned long index = (start_address / PAGE_SIZE) * sizeof(unsigned long);
	char* Data = (char*) malloc(1024);
	int pread_len = pread(fd, Data, len * sizeof(unsigned long), index);
	int dirtyCount = 0;
	if (pread_len < 0) {
		perror("pread");
		exit(-1);
	}
	for (int i = 0; i < len ; i++) {
		unsigned long entry = *(Data + i);
		if (entry >> 55 & 1) {
			dirtyPageVec.push_back((Address)start_address);
		}                                         
	}
	free(Data);
	return dirtyCount;
}

int getDirtyPages(std::vector<Address> dirtyPageVec) {
	// Open the pagemap for the pid
	char filename[] = "/proc/self/pagemap";

	int pagemapfd = open(filename, O_RDONLY, O_APPEND);
	if(pagemapfd < 0) {
		perror("open");
		fprintf(stderr, "ERROR!\n");
		exit(-1);
	}

	uint64_t stackVar = 0;
	// Let's skip the stack for now
	asm volatile ("movq %%rbp, %0"
			:"=r" (stackVar)::);

	int num = 0;
	//fprintf(stderr, "Stack var = %lx\n", stackVar);
	TargetAppMemory* targetAppMemory = TargetAppMemory::getTargetAppMemory();
	for (VMARegion* vma: targetAppMemory->getVMARegions()) {
		int res = readSoftDirty(pagemapfd, vma->startAddr, vma->endAddr, dirtyPageVec);
		num += res;
	}
	return num;
}

int clear_and_start_recording() {
	int clear_refs_file = open("/proc/self/clear_refs", O_WRONLY, O_APPEND);
	// Clear the soft-dirty bits
	char c = '4';
	write(clear_refs_file, &c, 1);
	close(clear_refs_file);
	return 0;
}

TargetAppMemory* TargetAppMemory::targetAppMemory = nullptr;
