#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"

Value *activePointers;


// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size){
	if (activePointers == NULL) {
		activePointers = malloc(sizeof(Value));
		activePointers->type = NULL_TYPE;
	}

  void *pointer = malloc(size);

  Value *node = malloc(sizeof(Value));
	node->type = PTR_TYPE;
	node->p = pointer;

	Value *consCell = malloc(sizeof(Value));
	consCell->type = CONS_TYPE;
	consCell->c.car = node;
	consCell->c.cdr = activePointers;
	activePointers = consCell;
  return pointer;
};

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree() {
	Value *next;
	if (activePointers == NULL) {
		
	} else {
		while (activePointers->type != NULL_TYPE) {
			next = activePointers->c.cdr;
			free(activePointers->c.car->p);
			free(activePointers->c.car);
    	free(activePointers);
    	activePointers = next;
		}
  	free(activePointers);
		activePointers = NULL;
	}
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {
	tfree();
	exit(0);
}
