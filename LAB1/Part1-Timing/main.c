#include "utility.h"

// TODO: Uncomment the following lines and fill in the correct size
#define L1_SIZE [32768]
#define L2_SIZE [262144]
#define L3_SIZE [8388608]
#define L1_WAYS [8]
#define L2_WAYS [8]
#define L3_WAYS [16]

int main (int ac, char **av) {

    // create 4 arrays to store the latency numbers
    // the arrays are initialized to 0
    uint64_t dram_latency[SAMPLES] = {0};
    uint64_t l1_latency[SAMPLES] = {0};
    uint64_t l2_latency[SAMPLES] = {0};
    uint64_t l3_latency[SAMPLES] = {0};

    // A temporary variable we can use to load addresses
    // The volatile keyword tells the compiler to not put this variable into a
    // register- it should always try to load from memory/ cache.
    volatile char tmp;

    // Allocate a buffer of 64 Bytes
    // the size of an unsigned integer (uint64_t) is 8 Bytes
    // Therefore, we request 8 * 8 Bytes
    uint64_t *target_buffer = (uint64_t *)malloc(8*sizeof(uint64_t));

    if (NULL == target_buffer) {
        perror("Unable to malloc target_buffer");
        return EXIT_FAILURE;
    }

    // [1.4] TODO: Uncomment the following line to allocate a buffer of a size
    // of your chosing. This will help you measure the latencies at L2 and L3.
    // Allocate a buffer of xx.
    float f = 1.5;
    uint64_t *eviction_buffer = (uint64_t *)malloc(f*32768*sizeof(uint64_t));
    if (NULL == eviction_buffer) {
	    perror("Unable to malloc eviction_buffer");
	    return EXIT_FAILURE;
    }

    // Example: Measure L1 access latency, store results in l1_latency array
    for (int i=0; i<SAMPLES; i++){
        // Step 1: bring the target cache line into L1 by simply accessing the line
        tmp = target_buffer[0];

        // Step 2: measure the access latency
        l1_latency[i] = measure_one_block_access_time((uint64_t)target_buffer);
    }

    // ======
    // [1.4] TODO: Measure DRAM Latency, store results in dram_latency array
    // ======
    //
    for (int i=0; i<SAMPLES; i++){
	    // Step 1: flush target address from caches if any
	    clflush(target_buffer);

	    // Step 2: measure the access latency
	    dram_latency[i] = measure_one_block_access_time((uint64_t)target_buffer);
    }

    // ======
    // [1.4] TODO: Measure L2 Latency, store results in l2_latency array
    // ======
    //
    int l1_lines = 512; //(L1_SIZE)/(LINE_SIZE);
    int reps_l1 = 100;

    for(int i=0; i<SAMPLES; i++){
	    
	    // Step 1: load target cache line into L1 (and consequently, to L2)
    	    tmp = target_buffer[0];

	    // Step 2: evict all of existing data in L1 by filling it with new addresses; do this reps_l1 times
	    for(int j=0; j<reps_l1; j++){
	    	for(int k=0; k<l1_lines; k++){
		    	tmp = eviction_buffer[k*8];
	    	}
   	    }
	    // Step 3: measure the access latency
	    l2_latency[i] = measure_one_block_access_time((uint64_t)target_buffer);
    }

    // ======
    // [1.4] TODO: Measure L3 Latency, store results in l3_latency array
    // ======
    //
    int l2_lines = 4096; //(L2_SIZE)/(LINE_SIZE)
    int reps_l2 = 100;
    
    for(int i=0; i<SAMPLES; i++){
    	    //printf("target buffer address: %ld\n", target_buffer);
	    //printf("eviction buffer address: %ld, %ld\n", &eviction_buffer[0], &eviction_buffer[1]);
	    //printf("diff: %ld", target_buffer - eviction_buffer);
	    
	    // Step 1: load target cache line into L1 (and consequently, to L2 and L3)
	    tmp = target_buffer[0];
	  
	    // Step 2: evict all of existing data in L2 by filling it with new addresses; do this reps_l2 times
	    for(int j=0; j<reps_l2; j++){
	    	for(int k=0; k<l2_lines; k++){
			tmp = eviction_buffer[k*8];
	    	}
	    }

	    // Step 3: measure the access latency
	    l3_latency[i] = measure_one_block_access_time((uint64_t)target_buffer);
    }

    // Print the results to the screen
    // [1.5] Change print_results to print_results_for_python so that your code will work
    // with the python plotter software
    print_results_for_python(dram_latency, l1_latency, l2_latency, l3_latency);

    free(target_buffer);

    // [1.4] TODO: Uncomment this line once you uncomment the eviction_buffer creation line
    free(eviction_buffer);
    return 0;
}

