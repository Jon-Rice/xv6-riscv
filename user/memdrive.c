#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/umalloc.h"

#define TARGET_MEMORY (64 * 1024 * 1024)  // 64 MB
#define MAX_MEMORY (120 * 1024 * 1024)    // 120 MB  
#define ALLOC_SIZE 128                    // Small allocations to create fragmentation

int main() {
    printf("Starting memdrive fragmentation test...\n");
    
    char *allocations[10000];
    int count = 0;
    uint64 total_allocated = 0;
    
    // Phase 1: Allocate many small blocks to fill pages
    printf("Phase 1: Allocating small blocks...\n");
    while(total_allocated < TARGET_MEMORY && count < 10000) {
        allocations[count] = malloc(ALLOC_SIZE);
        if(allocations[count] == 0) {
            printf("malloc failed at allocation %d\n", count);
            break;
        }
        total_allocated += ALLOC_SIZE;
        count++;
        
        // Progress indicator for large allocations
        if(count % 1000 == 0) {
            printf("Allocated %d blocks, total: %d bytes\n", count, total_allocated);
        }
    }
    
    printf("=== Initial Allocation Results ===\n");
    printf("Total allocations: %d\n", count);
    printf("Total requested memory: %d bytes\n", total_allocated);
    printf("Fragmented bytes: %d bytes\n", frag_bytes());
    printf("Fragmentation percentage: %.2f%%\n", 
           (frag_bytes() * 100.0) / total_allocated);
    
    // Phase 2: Free every other allocation to create fragmentation
    printf("\nPhase 2: Creating fragmentation pattern...\n");
    for(int i = 0; i < count; i += 2) {
        free(allocations[i]);
        allocations[i] = 0;
    }
    
    printf("=== After Fragmentation ===\n");
    printf("Remaining allocations: %d\n", count / 2 + (count % 2));
    printf("Used memory: %d bytes\n", (count / 2 + (count % 2)) * ALLOC_SIZE);
    printf("Fragmented bytes: %d bytes\n", frag_bytes());
    printf("Fragmentation percentage: %.2f%%\n", 
           (frag_bytes() * 100.0) / total_allocated);
    
    // Phase 3: Test allocation after fragmentation
    printf("\nPhase 3: Testing allocation after fragmentation...\n");
    char *test_allocs[10];
    int test_count = 0;
    
    for(int i = 0; i < 10; i++) {
        test_allocs[i] = malloc(512);  // Try medium-sized allocations
        if(test_allocs[i]) {
            test_count++;
        } else {
            printf("Failed to allocate 512 bytes (fragmentation impact)\n");
        }
    }
    
    printf("Successfully allocated %d/10 medium blocks despite fragmentation\n", test_count);
    
    // Cleanup
    for(int i = 0; i < 10; i++) {
        if(test_allocs[i]) {
            free(test_allocs[i]);
        }
    }
    
    // Final cleanup of original allocations
    for(int i = 0; i < count; i++) {
        if(allocations[i] != 0) {
            free(allocations[i]);
        }
    }
    
    printf("\nMemdrive test completed.\n");
    exit(0);
}
