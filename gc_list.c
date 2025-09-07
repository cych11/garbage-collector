#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "gc.h"

/* traverse the list data-structure and call mark_one(void *vptr) on
 *  each piece of memory that is still used in the list
 * head is the head of the list data-structure being traversed NOT
 * the head of the memory-list which is part of gc
 */
void mark_list(void *head) {
    struct node *curr = head;   // instantiate a node object given the memory address to a list
    // iterate over every node inside the list, calling mark_one and terminating once an unintended result was found
    while (curr != NULL) {
        int value = mark_one(curr);
        if (value != 0) {exit(value);}
        curr = curr->next;
    }
}
