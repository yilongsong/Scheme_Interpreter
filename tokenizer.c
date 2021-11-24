#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"

//All symbols which are recognized as part of a number
char numberElements[13] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                           '.', '+', '-'}; 

//All symbols that can derive from the nonterminal <initial>
char letterElements[66] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                           'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                           'U', 'V', 'W', 'X', 'Y', 'Z',
                           'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                           'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                           'u', 'v', 'w', 'x', 'y', 'z',
                           '!', '$', '%', '&', '*', '/', ':', '<', '=', '>',
                           '?', '~', '_', '^'};

//Returns a Value struct with the specified type and content
Value *makeValue(valueType type, char *content) {
    Value *value = talloc(sizeof(Value)); //allocate the struct
    value->type = type; //designate its type
    if (type == BOOL_TYPE || type == INT_TYPE) { //These types only need the integer
        value->i = atoi(content);
    } else if (type == DOUBLE_TYPE) { //This type only needs the decimal number
        value->d = strtod(content, NULL);
    } else { //All other value types contain a string, so they are allocated in the same way
        value->s = talloc(300); //Guaranteed character limit
        strcpy(value->s, content);
    }
    return value;
}

//Returns true if the object c points to belongs to the numberElements set
bool isNumberElement(char *c) {
    for(int i = 0; i < sizeof(numberElements)/sizeof(char); i++){
      if (*c == numberElements[i])
        return true;
    }
    return false;
}

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize() {
    char charRead; //The current character in the stream
    Value *list = makeNull(); //Start with an empty list
    charRead = (char)fgetc(stdin); //Get the first character 
    
    while (charRead != EOF) { //Until we reach the end of the file...
        if (charRead == ' '||charRead == '\n') {
            //We ignore newlines and whitespaces
        } else if (charRead == '#') { // We predict and match for a boolean expression
            charRead = (char)fgetc(stdin); //Get the next char (should be 't' or 'f')
            if (charRead == 'f') { //If f, assign 0 to this tokenized value
                list = cons(makeValue(BOOL_TYPE, "0"), list);
            } else if (charRead == 't') { //If t, assign 1
                list = cons(makeValue(BOOL_TYPE, "1"), list);
            } else { //If neither, throw an error because no rule tolerates #
                     //followed by anything other than 't' or 'f'
                printf("Syntax error: Invalid boolean indication following '#'.\n");
                texit(0);
            }
            
        } else if (isNumberElement(&charRead)) { 
            // Handles all characters deriving fromn the <identifier> nonterminal
            // We pass a pointer to prevent a compile-time error

            char content[300]; //String to create, assume maximum possible length
            int count = 0; //Used to keep track of our position in the string
            valueType type = INT_TYPE;
            
            //If it begins with the + | -, we can stop early
            if (charRead == '+' || charRead == '-') {
                char tmp = (char)fgetc(stdin); //Store the next digit as well
                if (tmp!='0'&&tmp!='1'&&tmp!='2'&&tmp!='3'&&tmp!='4'&tmp!='5'&&tmp!='6'&&tmp!='7'&&tmp!='8'&&tmp!='9') {
                    //If it's not just a negative/positive sign before a number as per the <subsequent> rule, then it's just a lone identifier terminal
                    //Create a string from charRead so it can be passed to makeValue()
                    content[0] = charRead;
                    content[1] = '\0';
                    //Add the cell to the list
                    list = cons(makeValue(SYMBOL_TYPE, content), list);
                    tmp = ungetc(tmp, stdin);
                    charRead = (char)fgetc(stdin);
                    continue;
                } else {
                    //If it is followed by a number, then we continue as normal
                    tmp = ungetc(tmp, stdin);
                }
            }
            //Iterate over every following number
            while (isNumberElement(&charRead)) {
                if (charRead == '.') {
                    //Designate the type if we encounter a '.', but throw an error if we
                    //reach this point twice
                    if (type == DOUBLE_TYPE) {
                        printf("Syntax error: More than one '.' in number.\n");
                        texit(0);
                    }
                    type = DOUBLE_TYPE;
                }
                //Otherwise, add the number to a string
                content[count] = charRead;
                count++;
                charRead = (char)fgetc(stdin);
            }
            //Terminate the string
            content[count] = '\0';
            //.. create the cell, and add it
            list = cons(makeValue(type, content), list);
            continue;
            
        } else if (charRead == '"') { // Handles string input
            charRead = (char)fgetc(stdin); //Advance, and add the " in manually
            char content[300]; //Assume maximum length
            content[0] = '"';
            int count = 1;
            //Add all successive characters to the string until we reach "
            while (charRead != '"') {
                content[count] = charRead;
                count++;
                charRead = (char)fgetc(stdin);
            }
            content[count] = '"';
            content[count+1] = '\0'; //Terminate the string, and create and add the cell
            
            list = cons(makeValue(STR_TYPE, content), list);
        
        //Handle open and close parens
        } else if (charRead == '(') { 
            list = cons(makeValue(OPEN_TYPE, "("), list);
            
        } else if (charRead == ')') { 
            list = cons(makeValue(CLOSE_TYPE, ")"), list);
            
        } else if (charRead == ';') { // comments to be stripped
            while (charRead != '\n' && charRead != EOF) { //Proceed to the next line/EOF
                charRead = (char)fgetc(stdin);
            }
            
        } else { //Assume input must be a symbol (we will error check it)
            // Handles the ' symbol
            if (charRead == 39) {
                char tmp = (char)fgetc(stdin);
                if (tmp == ' ' || tmp == '\n' || tmp == ')' || tmp == EOF) {
                    printf("Syntax error\n");
                    texit(0);
                } else {
                    tmp = ungetc(tmp, stdin);
                }
            }
            
            //Check to see if the next input is actually a letter (required for symbols)
            int i = 0;
            while (i < sizeof(letterElements) / sizeof(char)) {
              if(charRead == letterElements[i])
                break;
              i++;
            }
            //if i reached the limit, it's because we didn't find a matching valid letter
            if(i == sizeof(letterElements) / sizeof(char)){
              printf("Syntax error\n");
              texit(0);
            }

            
              char content[300]; //Create a string, assuming maximum length
              int count = 0; 
              while (charRead != ' ' && charRead != ')' && charRead != '\n' && charRead != EOF) { //Iterate through the rest of the word
                  content[count] = charRead;
                  count++;
                  charRead = fgetc(stdin);
                  //Add it to the string
              }
              content[count] = '\0';
              //Terminate the string, create a cell and add it to the list
              list = cons(makeValue(SYMBOL_TYPE, content), list);
              
              continue;
        }
        //Move to the next character
        charRead = (char)fgetc(stdin);
    }
    //Return the reversed list of tokens
    Value *revList = reverse(list);
    return revList;
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list) {
    Value *currentNode = list;
    while (currentNode->type != NULL_TYPE) {
        switch (car(currentNode)->type) {
            case BOOL_TYPE:
                if (car(currentNode)->i) {
                    printf("#t:boolean\n");
                } else {
                    printf("#f:boolean\n");
                }
                break;
            case INT_TYPE:
                printf("%d:integer\n", car(currentNode)->i);
                break;
            case DOUBLE_TYPE:
                printf("%lf:double\n", car(currentNode)->d);
                break;
            case STR_TYPE:
                printf("%s:string\n", car(currentNode)->s);
                break;
            case OPEN_TYPE:
                printf("(:open\n");
                break;
            case CLOSE_TYPE:
                printf("):close\n");
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", car(currentNode)->s);
                break;
            default:
                printf("Syntax Error\n");
            break;
        }
        currentNode = cdr(currentNode);
    }
}
