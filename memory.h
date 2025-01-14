#include <vector>
#include <cstdio>
#include <fcntl.h>    // For open, O_RDONLY, O_WRONLY, etc.
#include <unistd.h>   // For read, write, close
#include <cstdint>

typedef uint64_t Address;
struct VMARegion {
    uint64_t startAddr;
    uint64_t endAddr;
    uint64_t len;

    VMARegion(uint64_t start, uint64_t end) {
			startAddr = start;
			endAddr = end;
			len = end - start;
    }
};

class TargetAppMemory {
  private:
    std::vector<VMARegion*> VMARegions;
  public:
		void addVMARegion(VMARegion* vma) {
			VMARegions.push_back(vma);
		}

    void adjustVMAMunmap(Address page, size_t size) {
        // Multiple VMA regions might be affected by this
        Address munmapStart = page;
        Address munmapEnd = page + size;
				std::vector<VMARegion*>::iterator it = VMARegions.begin();
        while (it != VMARegions.end()) {
            VMARegion* vma = *it;
            if (munmapStart > vma->startAddr && munmapStart < vma->endAddr  && munmapEnd >= vma->endAddr) {
                // unmapped region spans partially across this vma
                // Adjust the VMA
                vma->endAddr = munmapStart;
                vma->len = vma->endAddr - vma->startAddr;
                it++;
            } else if (munmapStart <= vma->startAddr && munmapEnd >= vma->endAddr) {
                // unmapped region spans completely across this vma
                // This case also includes the simple case of
                // removing an entire VMA
                it = VMARegions.erase(it);
                delete(vma);
            } else if (munmapStart <= vma->startAddr && munmapEnd > vma->startAddr && munmapEnd < vma->endAddr) {
                // unmapped region spans partially across this vma
                // adjust the VMA
                vma->startAddr = munmapEnd;
                vma->len = vma->endAddr - vma->startAddr;
                it++;
            } else {
                it++;
            }
        }
    }

    TargetAppMemory() {
    }

    ~TargetAppMemory() {
    }

    static TargetAppMemory* targetAppMemory;

    static TargetAppMemory* getTargetAppMemory() {
      if (!targetAppMemory) {
        targetAppMemory = new TargetAppMemory();
      }
      return targetAppMemory;
    }

    std::vector<VMARegion*>& getVMARegions() {
      return VMARegions;
    }

    void dump(char* msg, char* lastInput, FILE* dumpFd) {
      fprintf(dumpFd, "\n%s: Input: %s\n", msg, lastInput);
      for (VMARegion* region: VMARegions) {
        fprintf(dumpFd, "region = %p - %p\n", (void*) region->startAddr, (void*) region->endAddr);
      }
      fprintf(dumpFd, "\nDONE %s\n", msg);
    }
};
