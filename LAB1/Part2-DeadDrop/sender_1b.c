
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

// TODO: define your own buffer size
#define BUFF_SIZE (1<<21)
//#define BUFF_SIZE [TODO]

int main(int argc, char **argv)
{
  // Allocate a buffer using huge page
  // See the handout for details about hugepage management
  void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }
  // The first access to a page triggers overhead associated with
  // page allocation, TLB insertion, etc.
  // Thus, we use a dummy write here to trigger page allocation
  // so later access will not suffer from such overhead.
  //*((char *)buf) = 1; // dummy write to trigger page allocation


  // TODO:
  // Put your covert channel setup code here

  //Create a write buffer of size L2 that should be written when input is 1.
  int f = 1.5;
  uint64_t *sender_buffer = (uint64_t *)malloc(f*32768*sizeof(uint64_t));
  if (NULL == sender_buffer){
	  perror("Unable to malloc sender buffer");
	  return EXIT_FAILURE;
  }

  volatile char tmp;
  int reps = 4;

  printf("Please type a message.\n");

  bool sending = true;
  while (sending) {
      char text_buf[128];
      
      fgets(text_buf, sizeof(text_buf), stdin);

      // TODO:
      // Put your covert channel code here
      //printf("input: %s", text_buf);
      
      //If input is 1, load write_buffer into cache
      if(*text_buf == '1'){
        	for(int j=0; j<4096; j++){
	  		tmp = sender_buffer[j*8];
     		}
      printf("Sent '1'\n");
      }
      //char *binary;
      //binary = string_to_binary(&text_buf);
      //printf("input-in-binary: %s\n", binary);
      
      //sending = false;
  }

  printf("Sender finished.\n");
  return 0;
}


