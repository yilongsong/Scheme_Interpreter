#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "interpreter.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>

// Prints Evaluation error and texits
void evaluationError() {
    printf("Evaluation error\n");
    texit(0);
}

// Prints given value
void printVal(Value *result) {
    switch (result->type) {
        case INT_TYPE: {
            printf("%d\n", result->i);
            break;
        }
        case DOUBLE_TYPE: {
            printf("%lf\n", result->d);
            break;
        }
        case STR_TYPE: {
            printf("%s\n", result->s);
            break;
        }
        case BOOL_TYPE: {
            if (result->i) {
                printf("#t\n");
            } else {
                printf("#f\n");
            }
            break;
        }
        case SYMBOL_TYPE: {
            printf("%s\n", result->s);
            break;
        }
        default: {
            evaluationError();
        }
    }
}

// Returns Value item of a given symbol
// Set tmp = frame->bindings.
// Then car(car(tmp)) is the string, car(cdr(car(tmp))) is the desired Value item.
Value *lookUpSymbol(Value *symbol, Frame *frame) {
		if (frame->bindings == NULL) {
				printf("Evaluation error: undefined symbol %s\n", symbol->s);
        texit(0);
		}
    Value *tmp = frame->bindings;
    Value valueOfSymbol;
    while (tmp->type != NULL_TYPE) {
				/*if (car(car(tmp))->type != SYMBOL_TYPE) {
						printf("Evaluation error: left side of a let pair doesn't have a variable\n");
						texit(0);
				}*/
        if (!strcmp(car(car(tmp))->s, symbol->s)) {
            if (car(cdr(car(tmp)))->type == SYMBOL_TYPE) {
								if (frame->parent == NULL) {
										printf("Evaluation error: undefined symbol %s\n", symbol->s);
        						texit(0);
								} else {
										return lookUpSymbol(car(cdr(car(tmp))), frame->parent);
								}	
            } else {
                return car(cdr(car(tmp)));
            }
        }
        tmp = cdr(tmp);
    }
		if (frame->parent == NULL) {
				printf("Evaluation error: undefined symbol %s\n", symbol->s);
        texit(0);
		}
		return lookUpSymbol(symbol, frame->parent);
}

void *getValue(Value *value, Frame *frame) {
    switch (value->type) {
        case INT_TYPE: {
            return &(value->i);
            break;
        }
        case DOUBLE_TYPE: {
            return &(value->d);
            break;
        }
        case STR_TYPE: {
            return &(value->s);
            break;
        }
        case BOOL_TYPE: {
            return &(value->i);
            break;
        }
        case SYMBOL_TYPE: {
            Value *symbol = lookUpSymbol(value, frame);
            return getValue(symbol, frame);
            break;
        }
        default: {
            evaluationError();
        }
    }
    void *null = NULL;
    return null;
}

Value *evalIf(Value *args, Frame *frame) {
    Value *tmp = args;
    
    // Check if args contains three arguments:
    int argCount = 0;
    while (tmp->type != NULL_TYPE) {
        argCount++;
        tmp = cdr(tmp);
        if (argCount > 3) {
            break;
        }
    }
    if (argCount != 3) {
        printf("Evaluation error: if has more or fewer than 3 arguments\n");
        texit(0);
    }
    
    if (*((int *) getValue(car(args), frame))) {
        return car(cdr(args));
    } else {
        return car(cdr(cdr(args)));
    }
}

//Evaluates the S-expression referred to by expr
//in the given frame.
//Recursive (?)
Value *eval(Value *expr, Frame *frame) {
    Value *result;
    switch (expr->type) {
        case INT_TYPE: {
            return expr;
            break;
        }
        case DOUBLE_TYPE: {
            return expr;
            break;
        }
        case STR_TYPE: {
            return expr;
            break;
        }
        case BOOL_TYPE: { // Not too sure
            return expr;
            break;
        }
        case SYMBOL_TYPE: {
            return lookUpSymbol(expr, frame);
            break;
        }
        case CONS_TYPE: {
            Value *first = car(expr);
            Value *args = cdr(expr);

            // Sanity and error checking on first...
            if (!strcmp(first->s, "if")) {
                return eval(evalIf(args, frame), frame);
            } else if (!strcmp(first->s, "let")) {
								//check for bad form
								if (car(args)->type != CONS_TYPE || car(car(args))->type != CONS_TYPE) {
										if (car(args)->type != CONS_TYPE) {
												printf("Evaluation error: bad form in let\n");
												texit(0);
										} else if (car(car(args))->type != CONS_TYPE && car(car(args))->type != NULL_TYPE) {
												printf("Evaluation error: bad form in let\n");
												texit(0);
										}
								}

                // create new frame
                Frame *newFrame = talloc(sizeof(Frame));
                newFrame->parent = frame;
                
                // bind symbols to values in car(args)
                newFrame->bindings = car(args);

								// check that each newFrame->bindings has correct number of arguments;
								// check that there's no duplicate variables in let;
								// check that each left param is a variable
								Value *argCheck = newFrame->bindings;
								Value *list = makeNull();
								int count = 0;
								while (argCheck->type != NULL_TYPE) {
										Value *arg = car(argCheck);
										if (arg->type == NULL_TYPE) {
											break;
										}
										if (car(arg)->type != SYMBOL_TYPE) {
												printf("Evaluation error: left side of a let pair doesn't have a variable\n");
												texit(0);
										}
										Value *tmp = list;
										while (tmp->type != NULL_TYPE) {
												if (!strcmp(car(arg)->s, car(tmp)->s)) {
														printf("Evaluation error: duplicate variable in let\n");
														texit(0);
												}
												tmp = cdr(tmp);
										}

										list = cons(car(arg), list);

										if (arg->type == NULL_TYPE) {
												break;
										}

										while (arg->type != NULL_TYPE) {
												count++;
												arg = cdr(arg);
										}
										if (count != 2) {
												printf("Evaluation error: first arg of let contains item(s) other than pairs\n");
												texit(0);
										}
										argCheck = cdr(argCheck);
										count = 0;
								}

                
                // evaluate second arg and more
                Value *tmp = cdr(args);
								if (tmp->type == NULL_TYPE) {
									printf("Evaluation error: no args following the bindings in let\n");
									texit(0);
								} 
                Value *eachExpr;
                while (tmp->type != NULL_TYPE) {
                    eachExpr = eval(car(tmp), newFrame);
                    tmp = cdr(tmp);
                }
                newFrame = newFrame->parent; // pop off frame
                return eachExpr;
            } else {
               // not a recognized special form
                evaluationError();
            }
        }
        default: {
            evaluationError();
        }
    }
    return result;
    
}

//Interprets each top level S-expression in the tree
//and prints out the results.
void interpret(Value *tree) {
    Frame *frame = talloc(sizeof(Frame));
		frame->bindings = NULL;
    // Empty (global) frame created.
    
    while (tree->type != NULL_TYPE) {
        printVal(eval(car(tree), frame));
        tree = cdr(tree);
    }
}
