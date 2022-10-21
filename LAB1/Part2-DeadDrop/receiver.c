
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

	volatile char tmp;
	uint64_t l2_threshold;
	uint64_t hit_time;
	uint64_t l1_l3_delta = 7;
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
		int hit = 0;
		int miss = 0;
		for(int i=0; i<lines; i=i+4){
			access_time = measure_one_block_access_time((uint64_t)(receiver_buffer+8*i));
			//printf("access_time = %d\t, threshold to cross for miss = %d\n", access_time, hit_time_average+l1_l3_delta);
			if(access_time > hit_time_average+l1_l3_delta){
				miss++;
			} else {
				hit ++;
			}
			access_time = measure_one_block_access_time((uint64_t)(receiver_buffer+8*(i+3)));
			if(access_time > hit_time_average+l1_l3_delta){
		 		miss++;						             	                } else {
												                        hit ++;
			}
			access_time = measure_one_block_access_time((uint64_t)(receiver_buffer+8*(i+1)));
                        if(access_time > hit_time_average+l1_l3_delta){
				miss++;
		        } else {
                                hit ++;
                        }
			access_time = measure_one_block_access_time((uint64_t)(receiver_buffer+8*(i+2)));
                        if(access_time > hit_time_average+l1_l3_delta){
                                miss++;
                        } else {
                                hit ++;
                        }
		}
		float miss_ratio;
	        miss_ratio = (float)miss/((float)hit + (float)miss);
		//printf("miss = %d\t, hit = %d\t, miss_ratio = %f\n", miss, hit, miss_ratio);
		if(miss_ratio > 0.26){
			printf("Received '1'. miss = %d, hit = %d, miss_ratio = %f\n", miss, hit, miss_ratio);
		}		
		//printf("Receiver finished.\n");
	}
	return 0;
}
