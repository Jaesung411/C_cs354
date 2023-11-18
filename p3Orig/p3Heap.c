////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap.c
// This File:        p3Heap.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2022
// Instructor:       deppeler
//
// Author:           Jaesung Lim
// Email:            jlim83@wisc.edu
// CS Login:         jaesung@best-linux.cs.wisc.edu
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   including Peer Mentors, Instructors, and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   Avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2022 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission Fall 2022, CS354-deppeler
//
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"
 
/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {           

    int size_status;

    /*
     * Size of the block is always a multiple of 8.
     * Size is stored in all block headers and in free block footers.
     *
     * Status is stored only in headers using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
} blockHeader;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 */


/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 * - If a BEST-FIT block found is NOT found, return NULL
 *   Return NULL unable to find and allocate block for desired size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {     
    //TODO: Your code goes in here.
    printf("%i will allocate\n",size); //for test
 
    //check size if not positive or if larger than heap space
    blockHeader *best_block = NULL;
    if(size <= 0 || size > alloc_size) {
	printf("larger\n");//for test
        return NULL;
    }

    //add the size of header
    int block_size = size + sizeof(blockHeader);

    //rounding up to a multiple of 8
    if(block_size % 8 != 0){
        block_size = block_size + (8 - block_size % 8);
    }

    //start from beginning block
    blockHeader *cur_block = heap_start;
   
    int valid = 0;
    while(valid == 0){
        //Allocated block of size about the best block and next block
      	int size_payload_cur = (cur_block->size_status/8)*8;
	
	int size_payload_best = 0;
	if(best_block != NULL){
		size_payload_best = (best_block->size_status/8)*8;
	}

        //check if the present block is free block and fits best
        if(cur_block->size_status % 2 == 0){
	       if(size_payload_cur >= block_size){
		       if(best_block == NULL){
				best_block = cur_block;
		       }else{
		       		if(size_payload_cur < size_payload_best){
					best_block = cur_block;
		      	 	}	
			}
	       }	       
        }

    	//update current block to go forward
	cur_block = (blockHeader*)((void*)cur_block + size_payload_cur); 
        
	//stop searching best fit block when reaching at the end mark
        if(cur_block->size_status == 1){    
	    valid = 1;
	    if(best_block == NULL){
		    return NULL;
	    }
        }

    }

    //Allocated block of size about the chosen best-fit free block
    int size_payload_best = (best_block->size_status/8)*8;
    int extra_block = size_payload_best - block_size;

    //when the found best-fit block is large enough to split
    if(extra_block >= 8){

        //make new next header for split
        blockHeader *next = (blockHeader*)((void*)best_block + block_size);

       	//make the status of status
       	next->size_status = extra_block;
       	next->size_status += 2; //p-bit

	//make footer for new free block in the end of the block
        blockHeader *footer = (blockHeader*)((void*)next + extra_block - sizeof(blockHeader));
        footer->size_status = extra_block;

        //update the status of the found best-fit block
        best_block->size_status = best_block->size_status - extra_block;
        
    }else{
	//the next block when not split    
	blockHeader *next = (blockHeader*)((void*)best_block + size_payload_best);

	//update the p-bit if the next block is not the end of heap
	if(next->size_status != 1){
		next->size_status += 2;
	}

    }
    
    //update a-bit of the allocated block
    best_block->size_status += 1;

    //display heap when a block is allocated
    disp_heap();
    
    printf("%p allocated \n",(void*)best_block); //for test
    return (void*)best_block + sizeof(blockHeader);
} 
 
/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 */                    
int bfree(void *ptr) {    
    //TODO: Your code goes in here.
    // Return -1 if ptr is NULL
    if(ptr == NULL){
	return -1;
    }

    //Return -1 if ptr is not a multiple of 8.
    if((int)ptr % 8 != 0){
	    return -1;
    }
    
    //Return -1 if ptr is outside of the heap space.
    if(ptr < (void*)heap_start || ptr > (void*)heap_start + alloc_size){
	return -1;
    }

    //Return -1 if ptr block is already free
    blockHeader *free_header = (blockHeader*)((void*)ptr - sizeof(blockHeader));
    int free_size = (free_header->size_status / 8) * 8;
    if(free_header->size_status % 2 == 0){
	    return -1;
    }

    //start free
    //change the a bit of freeing block
    free_header->size_status -= 1;

    //make footer for freeing block
    blockHeader *footer = (blockHeader*)((void*)free_header + free_size - sizeof(blockHeader));
    footer->size_status = free_size;

    //update the p-bit of the next block
    blockHeader *nextHeader = (blockHeader*)((void*)free_header + free_size);
    nextHeader->size_status -= 2;

    printf("%p,free\n",free_header); //for test    
    disp_heap(); //for test

    return 0;

} 

/*
 * Function for traversing heap block list and coalescing all adjacent 
 * free blocks.
 *
 * This function is used for delayed coalescing.
 * Updated header size_status and footer size_status as needed.
 */
int coalesce() {
    //TODO: Your code goes in here
    printf("coalesce\n");
    blockHeader *cur_block = heap_start;
    int check = 0;
    //keep going until the end of heap
    while(cur_block->size_status != 1){
	    int cur_size = (cur_block->size_status / 8) * 8;
	    blockHeader *footer = NULL;
	    //check if the current block is not free block
	    if(cur_block->size_status % 2 == 1){
		    cur_block = (blockHeader*)((void*)cur_block + cur_size);
	    }
	    //the current block is free block
	    else{
		    //check next block
		    blockHeader *next_header = (blockHeader*)((void*)cur_block + cur_size);
		    if(next_header->size_status % 2 == 0){
			    //update current block status
			    int next_size = (next_header->size_status / 8) * 8;
			    cur_block->size_status +=  next_size;
			    cur_size += next_size;

			    //update the footer of the coalesced block
			    footer = (blockHeader*)((void*)next_header + next_size - sizeof(blockHeader));
			    footer->size_status = cur_size;
				
			    //check if allocating
			    check = 1;
		    }	    
		    
		    //check previous block
		    if((cur_block->size_status & 2) == 0){
			//get the header of previous block   
			blockHeader *pre_footer = (blockHeader*)((void*)cur_block - sizeof(blockHeader));
			int pre_size = pre_footer->size_status;
			blockHeader *pre_header = (blockHeader*)((void*)cur_block - pre_size);

			//update the status of the coalesced block
			pre_header->size_status += cur_size;

			//update the footer of coalesced block
			footer = (blockHeader*)((void*)pre_header + pre_size + (cur_block->size_status/8) * 8 - sizeof(blockHeader));
			pre_footer->size_status = pre_size + (cur_block->size_status / 8) * 8;
		    	
			//check if allocating
			check = 1;
		    }
		   
		   //check next block
    		   cur_block = (blockHeader*)((void*)cur_block + cur_size);
	    }
    }

    disp_heap(); //for test

    //return 1 if coalesced 
    if(check == 1){
	    return 1;
    }

    //return 0 if not coalesced
    return 0;
}

 
/* 
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    
 
    static int allocated_once = 0; //prevent multiple myInit calls
 
    int pagesize;   // page size
    int padsize;    // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* end_mark;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
} 
                  
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     
 
    int counter;
    char status[6];
    char p_status[6];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout, 
	"*********************************** Block List **********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
	"---------------------------------------------------------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
	"---------------------------------------------------------------------------------\n");
    fprintf(stdout, 
	"*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
	"*********************************************************************************\n");
    fflush(stdout);

    return;  
} 


// end of myHeap.c (Spring 2022)                                         


