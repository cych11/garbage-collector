#include <stdio.h>
#include <stdlib.h>

#include "fstree.h"
#include "gc.h"

/* Traverse the fstree data-structure and call mark_one(void *vptr) on
 * each piece of memory that is still used in the fstree
 *
 * mark_one returns 0 if the chunk was marked for the first time and 1 if
 * it has been seen already. We need this to avoid revisiting pieces of the
 * tree that we have already marked -- where hard links have created cycles
 */

void mark_fstree(void *head) {
    // check case where head is NULL
    if (head == NULL) {return;}
    Fstree *curr = head;   // instantiate an fsnode object given the memory address to an fstree

    mark_one(curr); // mark the node itself
    if (curr->name) {mark_one(curr->name);} // mark the name of the node

    Link *curr_link = curr->links;   // instantiate a Link object pointing to the current nodes links
    while (curr_link != NULL) {
        mark_one(curr_link);    // mark the current link
        mark_fstree(curr_link->fptr); // Recursively mark child nodes
        curr_link = curr_link->next;    // move onto the next link
    }
}
