#include <stdio.h>
#include <stdlib.h>
#include "gc.h"

#include "list.h"

// global variable to be head of allocated pieces
struct mem_chunk *memory_list_head = NULL;

// global variable for debugging
int debug = 0;

void *gc_malloc(int nbytes) {
    int *memory = malloc(nbytes);   // malloc required space, checking if 0 was inputted
    if (memory == 0) {return NULL;}

    // malloc a mem_chunk object that represents a node for the memory list
    struct mem_chunk *new_mem_chunk = malloc(sizeof(struct mem_chunk));
    if (new_mem_chunk == 0) {   // ensure malloc call was used properly
        free(memory);
        return NULL;
    }

    new_mem_chunk->in_use = USED;
    new_mem_chunk->address = memory;    // assign new_mem_chunk the created memory address
    new_mem_chunk->next = memory_list_head; // make new_mem_chunk head of the list, memory_list_head becomes next node
    memory_list_head = new_mem_chunk;

    return memory;

}

/* Executes the garbage collector.
 * obj is the root of the data structure to be traversed
 * mark_obj is a function that will traverse the data structure rooted at obj
 *
 * The function will also write to the LOGFILE the results messages with the
 * following format strings:
 * "Mark_and_sweep running\n"
 * "Chunks freed this pass: %d\n"
 * "Chunks still allocated: %d\n"
 */


void mark_and_sweep(void *obj, void (*mark_obj)(void *)) {
    // open LOGFILE to append information regarding chunks freed and still allocated
    FILE *log_ptr = fopen(LOGFILE, "a");
    if (log_ptr == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int num_freed = 0, num_allocated = 0;   // variables used to track chunks freed and still allocated
    // instantiate mem_chunk objects that point to current and previous elements in the memory list
    struct mem_chunk *curr = memory_list_head, *prev = NULL;

    // 1. Reset: set all of the used flags to "not used" in the memory reference list.
    while (curr != NULL) {
        num_allocated++;
        curr->in_use = NOT_USED;
        curr = curr->next;
    }

    // 2. Mark: traverse the program's dynamically allocated data structures and set each address that we reach to
    // "used" in the memory reference list. Note that we need a different mark function for every data structure.
    // Cycles can be avoided by noting when a reference has already been marked as "in use" and halting the traversal
    // of that portion of the data structure.
    mark_obj(obj);

    // 3. Sweep: traverse the memory reference list, freeing each address that is not marked as "used", and removing
    // freed nodes from the list.
    curr = memory_list_head;

    while (curr != NULL) {
        struct mem_chunk *next = curr->next; // instantiate mem_chunk objects that point to the next element in the memory list
        if (curr->in_use == NOT_USED) {
            num_freed++;    // increment # of freed nodes
            num_allocated--;    // decrement # of allocated nodes

            free(curr->address);    // free current objects address
            free(curr); // free the node itself from the memory

            if (prev != NULL) {prev->next = next;}
            else {memory_list_head = next;}  // update head of the memory list if the first node is freed
        }
        else {prev = curr;} // no nodes were freed, move onto next node
        curr = next;
    }

    // write to the LOGFILE and then close the file pointer
    fprintf(log_ptr, "Mark_and_sweep running\nChunks freed this pass: %d\nChunks still allocated: %d\n", num_freed, num_allocated);
    fclose(log_ptr);
}


/*
 Mark the chunk of memory pointed to by vptr as in use.
 Return codes:
   0 if successful
   1 if chunk already marked as in use
   2 if chunk is not found in memory list

   Here is a print statement to print an error message:
   fprintf(stderr, "ERROR: mark_one address not in memory list\n");
 */
int mark_one(void *vptr) {
    struct mem_chunk *curr = memory_list_head;  // instantiate mem_chunk to be used inside the function

    while (curr != NULL) {  // iterate over the memory list to find the address given within the list
        if (curr->address == vptr) {
            if (curr->in_use == USED) {
                return 1;   // if the node has been found but was already used, return 1
            }

            curr->in_use = USED;    // if the node has been found and hasn't been already used, return 0
            return 0;
        }
        curr = curr->next;
    }
    return 2; // if the node has not been found, return 2
}

void print_memory_list() {
    struct mem_chunk *current = memory_list_head;
    while (current != NULL) {
        printf("%lx (%d) -> ",(unsigned long) current->address, current->in_use);
        current = current->next;
    }
    printf("\n");
}
