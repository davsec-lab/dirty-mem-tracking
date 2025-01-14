#include "memory.h"

int readSoftDirty(int, unsigned long, unsigned long, std::vector<Address>&);

int getDirtyPages(std::vector<Address>&);

int clear_and_start_recording();
