
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

int main(int argc, char **argv)
{
	// Put your covert channel setup code here
	
	uint64_t *receiver_buffer = (uint64_t *)malloc(32768*sizeof(uint64_t));
	if(NULL == receiver_buffer){
		perror("Unable to malloc receiver buffer");
		return EXIT_FAILURE;
	}

  uint64_t set_bits = 0;  
    
    //search for eviction set
    uint64_t* eviction_set;
    
    for(int i=0; i<32768; i++){
    	uint64_t val;
        val = receiver_buffer+i;
    	//printf("val = %p\n", val);
        if(set_bits == ((val & ((1 << 18)-1)) >> 6)){
    	    printf("for set index: %d, found eviction set: %p\n", set_bits, val);
            eviction_set = receiver_buffer+i;
            printf("double check. %p\n", eviction_set);
            break;
        }
    }		
	
	volatile char tmp;
	uint64_t l2_threshold;
	uint64_t hit_time;
	uint64_t l1_l3_delta = 15;
	uint64_t hit_time_average = 0;
	int reps = 10000;
	int lines = 4096;

	//prime L2
	for(int i=0; i<lines; i++){
		tmp = receiver_buffer[i*8];
	}
	//Calculate AMAT
	for(int i=0; i<reps; i++){
		hit_time = 0;
		for(int i=0; i<lines; i++){
			//tmp = receiver_buffer[i*8];
			hit_time = hit_time + measure_one_block_access_time((uint64_t)(receiver_buffer+i*8));
		}
	hit_time_average += hit_time/lines;
	}

	hit_time_average = hit_time_average/reps;
	l2_threshold = hit_time_average + l1_l3_delta;
	printf("AMAT on a hit: %d\n", hit_time_average);
	
	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	while (listening) {
		// Put your covert channel code here
		uint64_t access_time = 0;
		int hit[9] = {0,0,0,0,0,0,0,0,0};
		int miss[9] = {0,0,0,0,0,0,0,0,0};
		char msg[8] = "00000000";
		for(int i=0; i<9; i++){ //sets
			for(int j=0; j<8; j++){ //lines
				access_time = measure_one_block_access_time((uint64_t)(receiver_buffer+8*i+j));
				if(access_time > hit_time_average+l1_l3_delta){
					miss[i]++;
				} else {
					hit[i]++;
				}
			}
		}
		//printf("Message = ");
		for(int i=0; i<8; i++){
			if(miss[i] > 3){
				miss[i] = 1;
				msg[i] = '1';
			} else {
				miss[i] = 0;
				msg[i] = '0';
			}
			//printf("%d", miss[i]);
		}
		//printf("\nmsg=%s, \tcheck_bit_sum = %d", msg, miss[8]);
	
		//printf("\n");
		if(miss[8] > 6){
			char *message;
			message = binary_to_string(msg);
			printf("message = %s\n", message);
		}

	}

	//printf("Receiver finished.\n");	
	return 0;
}
