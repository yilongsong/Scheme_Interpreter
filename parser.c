#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include <stdio.h>
#include <assert.h>

// Helper function that adds a token to the parse tree correctly
Value *addToParseTree(Value *tree, int *depth, Value *token) {
		int d = *depth;
		Value *tmp = tree;
		Value *prev;

		while (d > 0) {
			while (cdr(tmp)->type != NULL_TYPE) {
				tmp = cdr(tmp);
			}
			tmp = car(tmp);
			d--;
		}
		if (tmp->type != NULL_TYPE) {
			while (cdr(tmp)->type != NULL_TYPE) {
				prev = tmp;
				tmp = cdr(tmp);
			}
		}// tmp now represents the last cons cell

		if (token->type == OPEN_TYPE) {
			if (tree->type == NULL_TYPE) {
				tree = cons(makeNull(), tree);
				tmp = tree;
			}

			if (car(tmp)->type != NULL_TYPE) {
				tmp->c.cdr = cons(makeNull(), makeNull());
				tmp = tmp->c.cdr;
			}
			tmp->c.car = cons(makeNull(), makeNull());
			*depth = *depth + 1;

		} else if (token->type == CLOSE_TYPE) {
			if (*depth == 0) {
				printf("Syntax error: too many close parentheses\n");
				texit(0);
			}
			*depth = *depth - 1;
		
		} else {
			//printf("tmp->type (when adding token): %u\n", tmp->type);
			if (tree->type == NULL_TYPE) {
				tree = cons(makeNull(), tree);
				tmp = tree;
			}

				if (car(tmp)->type != NULL_TYPE) {
					tmp->c.cdr = cons(makeNull(), makeNull());
					tmp = tmp->c.cdr;
				}
			tmp->c.car = token;
		}

		return tree;
}

// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {
    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");

    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }
    if (depth != 0) {
        printf("Syntax error: not enough close parentheses\n");
				texit(0);
    }

    return tree;
}

// Helper function that prints the content of a token
void printValue(Value *token, Value *next) {
	if (token->type == INT_TYPE) {
		if (next->type == NULL_TYPE) {
			printf("%d", token->i);
		} else {
			printf("%d ", token->i);
		}
	} else if (token->type == DOUBLE_TYPE) {
		if (next->type == NULL_TYPE) {
			printf("%lf", token->d);
		} else {
			printf("%lf ", token->d);
		}
	} else if (token->type == STR_TYPE) {
		if (next->type == NULL_TYPE) {
			printf("%s", token->s);
		} else {
			printf("%s ", token->s);
		}
		
	} else if (token->type == BOOL_TYPE) {
		if (token->i) {
			if (next->type == NULL_TYPE) {
				printf("#t");
			} else {
				printf("#t ");
			}
		} else {
			if (next->type == NULL_TYPE) {
			printf("#f");
		} else {
			printf("#f ");
		}
		}
	} else if (token->type == SYMBOL_TYPE) {
		if (next->type == NULL_TYPE) {
			printf("%s", token->s);
		} else {
			printf("%s ", token->s);
		}
	}
}

 void printTreeRecur(Value *tree, int *count, int *isNext) {
	if (tree->type == NULL_TYPE) {
		if (*count != 0) {
			printf(") ");
			*count = *count - 1;
		}
	} else if (tree->type == CONS_TYPE) {
		if (car(tree)->type == CONS_TYPE) {
			printf("(");
			*count = *count + 1;
			if (cdr(tree)->type != NULL_TYPE) {
				*isNext= 1;
			} else {
				*isNext = 0;
			}
			printTreeRecur(car(tree), count, isNext);
			if (isNext) {
				printTreeRecur(cdr(tree), count, isNext);
			}
		} else {
			Value *next = cdr(tree);
			printValue(car(tree), next);
			if (cdr(tree)->type == CONS_TYPE) {
			}
			printTreeRecur(cdr(tree), count, isNext);
		}
	}
} 


// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
	int zero = 0;
	int isNext = 0;
	printTreeRecur(tree, &zero, &isNext);
}
