#ifndef NYANCAT_NYA_PARSER_H
#define NYANCAT_NYA_PARSER_H

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDBOOL_H
#define STDBOOL_H
#include <stdbool.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

#include "operations.h"
#include "../utils/hashmap.h"
#include "../utils/list.h"

#define MAX_ERROR_LENGTH    1<<8
#define MAX_LINE_LENGTH     1<<8
#define MAX_ARGUMENT_LENGTH 1<<7
#define MAX_LABEL_LENGTH    1<<7
#define MAX_PARAM_LENGTH    1<<6

list *load_script(char *path);

typedef enum _nyanError {
    NO_ERRORS = -1,
    GENERAL_NOT_NYA,
    GENERAL_ILLEGAL_CHAR,
    ARGUMENT_ILLEGAL,
    ARGUMENT_UNKNOWN,
    ARGUMENT_DUPLICATED,
    ARGUMENT_NO_COMMA,
    ARGUMENT_UNNECESSARY,
    ARGUMENT_NO_ANGLE_BRACKET,
    ARGUMENT_LOOSE_REFERENCE,
    LABEL_ILLEGAL,
    LABEL_UNKNOWN,
    LABEL_DUPLICATED,
    TASK_ILLEGAL,
    TASK_UNKNOWN,
    TASK_UNEXPECTED_PARAM,
    TASK_MISSING_PARAMS,
    TASK_EXCESSIVE_PARAMS
} nyanError;

typedef enum _nyanLine {
    LINE_UNDEFINED = -1,
    LINE_ARGUMENT,
    LINE_LABEL,
    LINE_TASK,
} _nyanLine;

// Lexes a .nya script
void lex_script(FILE *f, list *a, int *errorId, hashmap *labels, hashmap *arguments, bool preBuild) {
    // Line buffer
    char line[MAX_LINE_LENGTH];
    // If the line is inside a block comment
    bool onBlockComment = false;
    
    // Loops through lines
    while(fgets(line, MAX_LINE_LENGTH, f)) { 
        // Char counter
        int j = 0;
        // Line definition
        int k = LINE_UNDEFINED;

        while(j < sizeof(line)
            && line[j] != '\0'
            && line[j] != '\n') {
            if(onBlockComment) {
                // Checks if the character ends block comment
                if(line[j] == '*'
                    && line[j + 1] == '/') {
                    // Ignores end comment notation
                    j += strlen("*/");
                    onBlockComment = false;
                }
                j++;
            } else {
                // Checks if the character starts block comment
                if(line[j] == '/'
                    && line[j + 1] == '*') {
                    // Ignores end comment notation
                    j += strlen("/*");
                    onBlockComment = true;
                    continue;
                }

                // Checks if the character is an in-line comment
                if(line[j] == '/'
                    && line[j + 1] == '/') {
                    break;
                }
                
                // Checks if the character is a whitespace
                if(line[j] == ' ') {
                    j++;
                    continue;
                }

                // Identifies tasks
                if(line[j] >= 'a'
                    && line[j] <= 'z') {
                    // Doesn't declare tasks on pre building
                    if(preBuild) break;

                    // Allocates a buffer for the name
                    char *taskBuffer = malloc(sizeof(char) * MAX_TASK_LENGTH);
                    taskBuffer[0] = '\0';
                    while(line[j] >= 'a'
                        && line[j] <= 'z' ) {
                        // Appends character to buffer array
                        sprintf(taskBuffer, "%s%c", taskBuffer, line[j]);
                        // Goes to next character
                        j++;
                    }
                    // Checks if found task is a known native task
                    int l;
                    for(l = sizeof nyanTasks / sizeof(nyanTask) - 1; l >= 0; l--) {
                        if(strcmp(nyanTasks[l].name, taskBuffer) == 0) break;
                    }
                    // If task is valid, start reading it's parameters
                    if(l >= 0) {
                        // Creates operation (parsing)
                        nyanOperation b = {l, { 0 }, { false }};
                        // If line has already another function, throw an error
                        if(*errorId == NO_ERRORS
                                && k != LINE_UNDEFINED) {
                            *errorId = TASK_ILLEGAL;
                            break;
                        }
                        // Makes line a task line
                        k = LINE_TASK;
                        // Parameter counter
                        int m = 0;
                        // Calculates desired parameter quantity
                        int n = 0;
                        while(nyanTasks[l].parameters[n] != TYPE_VOID
                            && n < MAX_PARAM_LENGTH) n++;
                        // Loops through line
                        while(j < sizeof(line)
                            && line[j] != '\0'
                            && line[j] != '\n') {
                            // If it's a whitespace, ignore it
                            while(line[j] == ' ') {
                                j++;
                                continue;
                            }
                            // If it's a number, identify it
                            if(line[j] >= '0'
                                && line[j] <= '9'){
                                // If the task is already filled with parameters, break
                                if(*errorId == NO_ERRORS
                                    && m >= n) {
                                    *errorId = TASK_EXCESSIVE_PARAMS;
                                    break;
                                }
                                // Allocates a buffer for the number name
                                char *numberBuffer =
                                    malloc(sizeof(char) * MAX_TASK_LENGTH);
                                numberBuffer[0] = '\0';
                                // Iterate until the character isn't a number
                                while(line[j] >= '0'
                                    && line[j] <= '9') {
                                    sprintf(numberBuffer, "%s%c",
                                        numberBuffer, line[j]);
                                    j++;
                                }
                                // Checks it's a classical register (reserved or not)
                                if(line[j] == '!'
                                    || line[j] == '%') {
                                        bool isReserved = line[j] == '%';
                                        // Ignores type character
                                        j++;
                                        // If the parameter ends properly
                                        if(j >= sizeof(line)
                                            || line[j] == '\0'
                                            || line[j] == '\n'
                                            || line[j] == ' ') {
                                            // Saves value into operation
                                            b.values[m] = atoi(numberBuffer);
                                            b.specials[m] = isReserved;
                                            // If task expected classical, continues
                                            if(nyanTasks[l].parameters[m] ==
                                                TYPE_BIT) {
                                                // Goes to the next parameter
                                                m++;
                                                continue;
                                            } else if(*errorId == NO_ERRORS) {
                                                *errorId = TASK_UNEXPECTED_PARAM;
                                                break;
                                            }
                                        }
                                // Checks if it's a quantum register
                                } else if(line[j] == '?') {
                                    // Ignores type character
                                    j++;
                                    // If the parameter ends properly
                                    if(j >= sizeof(line)
                                        || line[j] == '\0'
                                        || line[j] == '\n'
                                        || line[j] == ' ') {
                                        // If task expected quantum, continues
                                        if(nyanTasks[l].parameters[m] ==
                                            TYPE_QUBIT) {
                                            // Saves value into operation
                                            b.values[m] = atoi(numberBuffer);
                                            b.specials[m] = false;
                                            // Goes to the next parameter
                                            m++;
                                            continue;
                                        } else if(*errorId == NO_ERRORS) {
                                            *errorId = TASK_UNEXPECTED_PARAM;
                                            break;
                                        }
                                    }
                                } else if(j >= sizeof(line)
                                        || line[j] == '\0'
                                        || line[j] == '\n'
                                        || line[j] == ' ') {
                                    // If task expected value, continues
                                    if(nyanTasks[l].parameters[m] ==
                                        TYPE_VAL) {
                                        // Saves value into operation
                                        b.values[m] = atoi(numberBuffer);
                                        b.specials[m] = false;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if(*errorId == NO_ERRORS) {
                                        *errorId = TASK_UNEXPECTED_PARAM;
                                        break;
                                    }
                                }
                            }
                            // If it's an argument, identify it
                            if(line[j] == '_'
                                || (line[j] >= 'a'
                                    && line[j] <= 'z')) {
                                // If the task is already filled with parameters, break
                                if(*errorId == NO_ERRORS
                                    && m >= n) {
                                    *errorId = TASK_EXCESSIVE_PARAMS;
                                    break;
                                } else if (*errorId == NO_ERRORS
                                    && nyanTasks[l].parameters[m] != TYPE_VAL) {
                                    // If task didn't expected this value, continues
                                    *errorId = TASK_UNEXPECTED_PARAM;
                                    break;
                                }
                                // Allocates a buffer for the argument name
                                char *argumentBuffer =
                                    malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
                                argumentBuffer[0] = '\0';
                                // Iterate until the character isn't valid
                                while(line[j] == '_'
                                || (line[j] >= 'a'
                                    && line[j] <= 'z')
                                || (line[j] >= 'A'
                                    && line[j] <= 'Z')) {
                                    // Appends character to buffer array
                                    sprintf(argumentBuffer, "%s%c",
                                        argumentBuffer, line[j]);
                                    j++;
                                }
                                // If the parameter ends properly
                                if(j >= sizeof(line)
                                    || line[j] == '\0'
                                    || line[j] == '\n'
                                    || line[j] == ' ') {
                                    // Checks if the argument was delcared previously
                                    int value = get_val_from_hashmap(arguments, 
                                        argumentBuffer);
                                    // If it exists, replace it with it's value
                                    if(value >= 0){
                                        // Saves value into operation
                                        b.values[m] = value;
                                        b.specials[m] = true;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if (*errorId == NO_ERRORS) {
                                        *errorId = ARGUMENT_UNKNOWN;
                                        break;
                                    }
                                }
                            }
                            // If it's a label, identify it
                            if(line[j] >= 'A'
                                && line[j] <= 'Z') {
                                // If the task is already filled with parameters, break
                                if(*errorId == NO_ERRORS
                                    && m >= n) {
                                    *errorId = TASK_EXCESSIVE_PARAMS;
                                    break;
                                } else if (*errorId == NO_ERRORS
                                    && nyanTasks[l].parameters[m] != TYPE_LABEL) {
                                    // If task didn't expected this value, breaks
                                    *errorId = TASK_UNEXPECTED_PARAM;
                                    break;
                                }
                                // Allocates a buffer for the argument name
                                char *labelBuffer =
                                    malloc(sizeof(char) * MAX_LABEL_LENGTH);
                                labelBuffer[0] = '\0';
                                // Iterate until the character isn't valid
                                while(line[j] == '_'
                                || (line[j] >= 'a'
                                    && line[j] <= 'z')
                                || (line[j] >= 'A'
                                    && line[j] <= 'Z')) {
                                    // Appends character to buffer array
                                    sprintf(labelBuffer, "%s%c",
                                        labelBuffer, line[j]);
                                    j++;
                                }
                                // If the parameter ends properly
                                if(j >= sizeof(line)
                                    || line[j] == '\0'
                                    || line[j] == '\n'
                                    || line[j] == ' ') {
                                    // Checks if the label exists
                                    int value = get_val_from_hashmap(labels, 
                                        labelBuffer);
                                    // If it exists, replace it with it's value
                                    if(value >= 0) {
                                        // Saves value into operation
                                        b.values[m] = value;
                                        b.specials[m] = false;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if (*errorId == NO_ERRORS) {
                                        *errorId = LABEL_UNKNOWN;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        if(m == n) {
                            put_val_on_list(a, b);
                            continue;
                        } else if(*errorId == NO_ERRORS){
                            *errorId = TASK_MISSING_PARAMS;
                            break;
                        }
                    } else {
                        // Checks if programmer actually meant to reference an argument
                        int index = get_val_from_hashmap(arguments,
                            taskBuffer);
                        
                        if(index >= 0) {
                            if(*errorId == NO_ERRORS) {
                                *errorId = ARGUMENT_LOOSE_REFERENCE;
                                break;
                            }
                        } else if(*errorId == NO_ERRORS) {
                            *errorId = TASK_UNKNOWN;
                            break;
                        }
                    }
                }

                // Identifies declaring labels
                if(line[j] >= 'A'
                    && line[j] <= 'Z') {
                    // If line has already another function, throw an error
                    if(*errorId == NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        *errorId = LABEL_ILLEGAL;
                        break;
                    }
                    // Doesn't declare labels on building
                    if(!preBuild) break;
                    // Makes line a label declaration line
                    k = LINE_LABEL;
                    // Allocates a buffer for the name
                    char *labelBuffer = malloc(sizeof(char) * MAX_LABEL_LENGTH);
                    labelBuffer[0] = '\0';
                    // Loops through the name
                    while((line[j] >= 'a'
                            && line[j] <= 'z')
                        || (line[j] >= 'A'
                            && line[j] <= 'Z')
                        || (line[j] >= '0'
                            && line[j] <= '9')) {
                        // Appends character to buffer array
                        sprintf(labelBuffer, "%s%c", labelBuffer, line[j]);
                        // Goes to next character
                        j++;
                    }
                    if(line[j] == '\0'
                        || line[j] == '\n'
                        || line[j] == ' ') {
                        bool success = put_val_on_hashmap(
                            labels,
                            labelBuffer,
                            a->size
                        );
                        if(*errorId == NO_ERRORS
                            && !success) {
                            *errorId = LABEL_DUPLICATED;
                            break;
                        }
                        // Frees buffer
                        free(labelBuffer);
                        continue;
                    }
                }

                // Identifies opening arguments
                if(line[j] == '<') {
                    // Doesn't declare arguments on pre building
                    if(preBuild) break;
                    // If line has already another function, throw an error
                    if(*errorId == NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        *errorId = ARGUMENT_ILLEGAL;
                        break;
                    }
                    // Makes line a argument declaration line
                    k = LINE_ARGUMENT;
                    // If the arguments declaration has been closed already
                    bool closedDeclaration = false;
                    // If any arguments has been found
                    bool foundArgument = false;
                    // If a whitespace has been found
                    bool foundWhitespace = false;
                    // Ignores opening character
                    j++;
                    // Allocates a buffer for the name
                    char *argumentBuffer = malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
                    argumentBuffer[0] = '\0';
                    do {
                        if(line[j] == ' ') {
                            foundWhitespace = true;
                            j++;
                        }
                        // If it's a valid separator, records argument
                        if(line[j] == ','
                            || line[j] == '>') {
                            if(foundArgument) {
                                bool success = put_val_on_hashmap(
                                    arguments,
                                    argumentBuffer,
                                    arguments->size
                                );
                                // If it's duplicated, throw error
                                if(*errorId == NO_ERRORS
                                    && !success) {
                                    *errorId = ARGUMENT_DUPLICATED;
                                    break;
                                }
                                // Finishes argument declaration
                                foundArgument = false;
                                // Closes declaration if found closing bracket
                                if(line[j] == '>')
                                    closedDeclaration = true;
                                // Ignores separator character
                                j++;
                                // Resets buffer
                                argumentBuffer[0] = '\0';
                            } else if (*errorId == NO_ERRORS) {
                                *errorId = ARGUMENT_UNNECESSARY;
                                break;
                            }
                        }
                        // If it matches ^:[a-zA-Z0-9], enters the argument loop
                        while(!closedDeclaration
                            && (line[j] >= 'a'
                                && line[j] <= 'z')
                            || (line[j] >= 'A'
                                && line[j] <= 'Z')
                            || (line[j] >= '0'
                                && line[j] <= '9')
                            || line[j] == '_') {
                            // Report error if last param is unfinished
                            if(*errorId == NO_ERRORS
                                && foundArgument
                                && foundWhitespace) {
                                *errorId = ARGUMENT_NO_COMMA;
                                break;
                            }
                            // Current character is not a whitespace
                            foundWhitespace = false;
                            // Argument declaration found
                            if(!foundArgument) foundArgument = true;
                            // Appends character to buffer array
                            sprintf(argumentBuffer, "%s%c", argumentBuffer, line[j]);
                            // Goes to next character
                            j++;
                        }
                    } while(!closedDeclaration
                        && (line[j] == ' '
                        || line[j] == ','
                        || line[j] == '>'));
                    // Frees buffer
                    free(argumentBuffer);
                    if(j >= strlen(line)
                        || line[j] == '\0'
                        || line[j] == '\n') {
                        // If there's no closing bracket, throw error
                        if(*errorId == NO_ERRORS
                            && !closedDeclaration) {
                            *errorId = ARGUMENT_NO_ANGLE_BRACKET;
                            break;
                        }
                    }
                    continue;
                }
                
                // Identifies character as loose
                if(*errorId == NO_ERRORS) {
                    printf("a: %c\n", line[j]);
                    *errorId = GENERAL_ILLEGAL_CHAR;
                }
                break;
            }
        }
            
    }
}

// Loads a .nya script
list *load_script(char *path) {
    // Build Error
    int errorId = NO_ERRORS;
    // General specials
    list *a = new_list();

    // Parameters
    hashmap *params = new_hashmap();
    // Labels
    hashmap *labels = new_hashmap();
    
    char *finalPath = malloc(sizeof(char) * (strlen(path) + strlen(".nya")));
    sprintf(finalPath, "%s%s", path, ".nya");
    FILE *f = fopen(finalPath, "r");

    // If opened file, starts building
    if(f) {
        // Pre building (finding labels)
        lex_script(f, a, &errorId, labels, params, true);
        // Rewinds the file
        rewind(f);
        // Actual building
        lex_script(f, a, &errorId, labels, params, false);
    } else {
        errorId = GENERAL_NOT_NYA;
    }
    
    fclose(f);
    if(errorId < 0) {      
        printf("nyancat: build succeeded c:\n");
        int i = 0;
        listnode *temp = a->first;
        while(temp && i < a->size) {
            printf("\t%i|%s:", i, nyanTasks[temp->val.code].name);
            // Calculates desired parameter quantity
            int j = 0, k;
            while(j < MAX_PARAM_LENGTH
                && nyanTasks[temp->val.code].parameters[j] != TYPE_VOID) j++;
            for(k = 0; k < j; k++) {
                // Names parameter type
                char *name;
                switch(nyanTasks[temp->val.code].parameters[k]) {
                    case TYPE_LABEL:
                        name = "line";
                        break;
                    case TYPE_QUBIT:
                        name = "qubit";
                        break;
                    case TYPE_BIT:
                        name = temp->val.specials[k]
                            ? temp->val.values[k]
                                ? "ARR" : "TRR" : "register";
                        break;
                    case TYPE_VAL:
                        name = temp->val.specials[k]
                            ? "argument" : "value";
                        break;
                    default: name = "\0";
                }
                // Prints parameter
                printf("\t%i (%s)", temp->val.values[k], name);
            }
            printf("\n");
            temp = temp->next;
            i++;
        }
    } else {
        char *errorMsg;
        switch(errorId) {
            case GENERAL_NOT_NYA:
                errorMsg = "There isn't a .nya script named that way.";
                break;
            case GENERAL_ILLEGAL_CHAR:
                errorMsg = "There's a purposeless loose character on your code.";
                break;
            case ARGUMENT_ILLEGAL:
                errorMsg = "It isn't allowed to declare arguments that way! (Maybe you're doing two stuff in one line?)";
                break;
             case ARGUMENT_UNKNOWN:
                errorMsg = "It doesn't seem like there's a declared argument with that name.";
                break;
            case ARGUMENT_DUPLICATED:
                errorMsg = "There's a cloned argument.";
                break;
            case ARGUMENT_NO_COMMA:
                errorMsg = "Your arguments declaration is missing a comma!";
                break;
            case ARGUMENT_UNNECESSARY:
                errorMsg = "There's an unecessary character on your arguments declaration. Take it out!";
                break;   
            case ARGUMENT_NO_ANGLE_BRACKET:
                errorMsg = "It seems that you forgot to \"close\" your arguments declaration.";
                break;  
            case ARGUMENT_LOOSE_REFERENCE:
                errorMsg = "There's a loose argument on your code. Did you mean to call a task?";
                break;    
            case LABEL_ILLEGAL:
                errorMsg = "It isn't allowed to declare labels that way! (Maybe you're doing two stuff on the same line?)";
                break;
            case LABEL_UNKNOWN:
                errorMsg = "It doesn't seem like there's a label with that name.";
                break;
            case LABEL_DUPLICATED:
                errorMsg = "There's a cloned label.";
                break;
            case TASK_ILLEGAL:
                errorMsg = "It isn't allowed to call tasks that way! (Maybe you're doing two stuff on the same line?)";
                break;
            case TASK_UNKNOWN:
                errorMsg = "It doesn't seem like there's a task with that name.";
                break;
            case TASK_UNEXPECTED_PARAM:
                errorMsg = "The called task doesn't ask for that type of parameter.";
                break;
            case TASK_MISSING_PARAMS:
                errorMsg = "The task you've called asks for more parameters than you provided.";
                break;
            case TASK_EXCESSIVE_PARAMS:
                errorMsg = "Too much parameters for the task you've called.";
                break;
            default:
                errorMsg = "Unknown error. What are you doing there? e_e";
                break;
        }
        printf("nyancat: build failed :c\n");
        printf("error: %s\n", errorMsg);
    }
    return a;
}
#endif