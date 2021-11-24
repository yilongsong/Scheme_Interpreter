#ifndef _INTERPRETER
#define _INTERPRETER

//Interprets each top level S-expression in the tree
//and prints out the results.
void interpret(Value *tree);

//Evaluates the S-expression referred to by expr
//in the given frame.
Value *eval(Value *expr, Frame *frame);

#endif

