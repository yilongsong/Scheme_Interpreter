#include <assert.h>
#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "talloc.h"

// Create a new NULL_TYPE value node.
Value *makeNull() {
	Value *node = talloc(sizeof(Value));
	node->type = NULL_TYPE;
	return node;
}

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
// Make sure have <assert.h> in your #include statements in linkedlist.c in order
// to use assert (see assignment for more details).
Value *car(Value *list) {
	assert(CONS_TYPE == list->type);
	return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {
	assert(CONS_TYPE == list->type);
	return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value) {
	//assert(value != NULL);
	return value->type == NULL_TYPE;
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value) {
	if (value->type == NULL_TYPE) {
		return 0;
	};

	return 1 + length(cdr(value));
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr) {
	Value *node = talloc(sizeof(Value));
	node->type = CONS_TYPE;
	node->c.car = newCar;
	node->c.cdr = newCdr;
	return node;
}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list){
  printf("( ");
  Value* currentNode = list;
  while (currentNode->type != NULL_TYPE) {
    switch (car(currentNode)->type){
      case INT_TYPE:
        printf("%d ", car(currentNode)->i);
        break;
      case DOUBLE_TYPE:
        printf("%lf ", car(currentNode)->d);
        break;
      case STR_TYPE:
        printf("%s ", car(currentNode)->s);
				break;
      default:
        printf("Error, invalid list\n");
      break;
    }
    currentNode = cdr(currentNode);
  }  
  printf(")\n");
};

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverseHelper(Value *list, Value *reversed) {
	if (isNull(list)) {
		return reversed;
	}
	reversed = cons(car(list), reversed);
  return reverseHelper(cdr(list), reversed);
}

Value *reverse(Value *list) {
	Value *head = makeNull();
  return reverseHelper(list, head);
}


