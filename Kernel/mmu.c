#include <mmu.h>
#include <stdio.h>
#include <pmm.h>
#include <sched.h>

#define PAGES_PER_THREAD 2
#define TOTAL_PAGES ((SCHED_MAX_PROC) * PAGES_PER_THREAD)

void* pages[TOTAL_PAGES];
int bytes_used[TOTAL_PAGES];

void free_thread_pages(int pid) {
	int cur, first, last;
	cur = first = pid * PAGES_PER_THREAD;
	last = first + PAGES_PER_THREAD;

	while (cur < last) {
		if(pages[cur] != NULL) {
			vmm_free_pages(pages[cur], VMM_PAGE_SIZE);
			pages[cur] = NULL;
			bytes_used[cur] = 0;
		}
		cur++;
	}
}

void* syscall_mmap(void* addr, uint64_t size) {
	if (size > VMM_PAGE_SIZE) {
		return NULL;
	}

	uint64_t pid = (uint64_t)sched_getpid();

	void* ret_addr;

	int cur, first, last;
	cur = first = pid * PAGES_PER_THREAD;
	last = first + PAGES_PER_THREAD;

	while (cur < last) {
		// check if page wasnt allocated
		if (pages[cur] == NULL) {
			int result = vmm_alloc_pages_from((void*)0x43000000, VMM_PAGE_SIZE, MASK_WRITEABLE | MASK_USER, &ret_addr);
			if (result) {
				pages[cur] = ret_addr;
				bytes_used[cur] = size;
				printf("Allocing at page %d of thread %d, bytes left: %d\n", cur - pid * PAGES_PER_THREAD, pid, VMM_PAGE_SIZE - bytes_used[cur]);
				return ret_addr;
			} else {
				return NULL;
			}
		} else {
			//check if it has enough space
			if (VMM_PAGE_SIZE - bytes_used[cur] >= size) {
				ret_addr = (void*)((uint64_t) pages[cur] + bytes_used[cur]);
				bytes_used[cur] += size;
				printf("Using page %d of thread %d, bytes left: %d\n", cur - pid * PAGES_PER_THREAD, pid, VMM_PAGE_SIZE - bytes_used[cur]);
				return ret_addr;
			}
		}
		cur++;
	}
	return NULL;
}

void syscall_munmap(void* addr, uint64_t size) {

	uint64_t page_num = (uint64_t)addr / VMM_PAGE_SIZE;
	void* page_addr = (void*) (page_num * VMM_PAGE_SIZE);

	uint64_t pid = (uint64_t)sched_getpid();

	int cur, first, last;
	cur = first = pid * PAGES_PER_THREAD;
	last = first + PAGES_PER_THREAD;

	while (cur < last) {
		if (pages[cur] == page_addr) {
			if (bytes_used[cur] - size <= 0) {
				vmm_free_pages(pages[cur], VMM_PAGE_SIZE);
				bytes_used[cur] = 0;
				pages[cur] = NULL;
			} else {
				bytes_used[cur] -= size;
			}
		}
		cur++;
	}
}

block* base_addr = NULL;

void * mmu_kmalloc(uint64_t size){

	if (size > VMM_PAGE_SIZE) {
		printf("Way too big bro\n");
		return 0;
	}

	if(size < 0){
		size = 0;
	}

	block* current_block, *last_block;
	
	if(base_addr){

		// Find a block starting from base address
		last_block = base_addr;
		current_block = find_block(&last_block, size);
		if(current_block){

			// Try to split the current block, using what is just necessary
			if(current_block->size - size >= BLOCK_SIZE + MIN_ALLOC_SIZE){
				split_block(current_block, size);
			}
			current_block->free = 0;
		}else{

			// Expand heap because it didnt find any available block to split
			current_block = expand_heap(last_block, size);
			if(!current_block){
				// Couldn't expand heap	
				return NULL;
			}
		}
	}else{
		// First initialization
		current_block = expand_heap(NULL, size);
		if(!current_block){
			// Couldn't expand heap	
			return NULL;
		}
	}
	return (void *)(current_block + 1);
}

block* find_block(block** last_block, uint64_t size){

	// Find the first free block with enough space or return null
	block* cur_block = base_addr;
	while(cur_block && !(cur_block->free && cur_block->size >= size)){
		*last_block = cur_block;
		cur_block = cur_block->next;
	}
	return cur_block;
}

block* split_block(block* b, uint64_t size){
	
	// New block metadata
	block* new_block;
	new_block = (block*)((uint64_t)(b + 1) + size);
	new_block->next = b->next;
	new_block->prev = b;
	new_block->free = 1;
	new_block->size = b->size - size - BLOCK_SIZE;

	//Update old block
	b->next = new_block;
	b->size = size;
	if(new_block->next){
		new_block->next->prev = new_block;
	}
	return b;
}

block* expand_heap(block* last_block, uint64_t size){

	// Mmap enough for the new block and requested size
	void* result = gmem();

	if(result == NULL){
		return NULL;
	}
	
	// Create metadata block
	block* new_block;
	new_block = (block *)result;
	new_block->size = size;
	new_block->free = 0;
	new_block->prev = last_block;
	new_block->next = NULL;

	// Update last block references
	if(last_block){
		last_block->next = new_block;
	}else{
		base_addr = new_block;
	}

	// create extra free block if space is available
	int free_bytes = VMM_PAGE_SIZE % size;
	if (free_bytes > BLOCK_SIZE + MIN_ALLOC_SIZE && free_bytes < 4096) {
		block* free_block;
		free_block = (block *)((uint64_t) result + size + BLOCK_SIZE);
		free_block->size = free_bytes - BLOCK_SIZE;
		free_block->free = 1;
		free_block->prev = new_block;
		free_block->next = NULL;
		new_block->next = free_block;
	}

	return new_block;	
}

void mmu_kfree(void * address){

	// Check if its valid and get the actual block
	block* block_to_free;
	block_to_free = get_block(address);
	if (block_to_free == NULL){
		printf("Block doesnt exist\n");
	}
	block_to_free->free = 1;

	if(!block_to_free->next){			
		// If it was the last block, make sure prev points to null
		if (block_to_free->prev){
			block_to_free->prev->next = NULL;
		}else{
			// Has no next nor previous so heap is empty
			base_addr = NULL;
		}

		// Unmap since it was the last block
		//fmem((void*)block_to_free);
	}
}

block* get_block(void * address){

	// Check if the address is a block in the structure
	block* cur_block = base_addr;
	while (cur_block){
		if (cur_block+1 == address){
			return cur_block;
		}
		cur_block = cur_block->next;
	}
	return NULL;
}

void mmu_print_kheap() {
	block* cur_block = base_addr;
    
    if (!cur_block){
        printf("Heap is empty bro\n");
        return;
    }

    while (cur_block){
        printf("Block address: %d\nData address: %d\nBlock size: %d\nPrev block: %d\nNext block: %d\nFree: %d\nString: %s\n\n", cur_block, cur_block + 1, cur_block->size, cur_block->prev, cur_block->next, cur_block->free, cur_block + 1);
        cur_block = cur_block->next;
    }
}