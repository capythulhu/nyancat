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

#define MAX_LINE_LENGTH     1<<8
#define MAX_ARGUMENT_LENGTH 1<<7
#define MAX_LABEL_LENGTH    1<<7
#define MAX_PARAM_LENGTH    1<<6
typedef struct _qscript {
    list *algorithm;
    int qtotal;
    int ctotal;
} qscript;

typedef enum _nyanLine {
    LINE_UNDEFINED = -1,
    LINE_ARGUMENT,
    LINE_LABEL,
    LINE_TASK,
} _nyanLine;

void lex_script(FILE *f, qscript *r, int *errorId, hashmap *labels, hashmap *arguments, bool preBuild);
qscript load_script(char *path, bool echo);

// Lexes a .nya script
void lex_script(FILE *f, qscript *r, nyanBuildError *errorId, hashmap *labels, hashmap *arguments, bool preBuild) {
    // Line buffer
    char line[MAX_LINE_LENGTH];
    // If the line is inside a block comment
    bool onBlockComment = false;
    // Task count on pre build
    int p = 0;
    
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
                    if(preBuild) {
                        // Increments task count
                        p++;
                        break;
                    }
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
                        if(*errorId == BUILD_ERR_NO_ERRORS
                                && k != LINE_UNDEFINED) {
                            *errorId = BUILD_ERR_TASK_ILLEGAL;
                            break;
                        }
                        // Makes line a task line
                        k = LINE_TASK;
                        // Valid parameters counter
                        int m = 0;
                        // Calculates desired parameter quantity
                        int n = 0;
                        while(nyanTasks[l].parameters[n] != TYPE_VOID
                            && n < MAX_PARAM_LENGTH) n++;
                        // Theoretical parameters counter
                        int o = 0;
                        // Loops through line
                        while(j < sizeof(line)
                            && line[j] != '\0'
                            && line[j] != '\n'
                            && line[j] != '/') {
                            // If it's a whitespace, ignore it
                            while(line[j] == ' ') {
                                j++;
                                continue;
                            }
                            // If it's a reference, identify it
                            if(line[j] == '['){
                                bool closedReference = false;
                                bool foundNumber = false;
                                // Ignores opening character
                                j++;
                                // Increments theoretical parameters count
                                o++;
                                // If the task is already filled with parameters, break
                                if(*errorId == BUILD_ERR_NO_ERRORS
                                    && o > n) {
                                    *errorId = BUILD_ERR_TASK_EXCESSIVE_PARAMS;
                                    break;
                                }
                                // Allocates a buffer for the number name
                                char *numberBuffer =
                                    malloc(sizeof(char) * MAX_NUM_LENGTH);
                                numberBuffer[0] = '\0';
                                // If it's a whitespace, ignore it
                                while(line[j] == ' ') j++;
                                // Iterate until the character isn't a number
                                while(line[j] >= '0'
                                    && line[j] <= '9') {                                    
                                    sprintf(numberBuffer, "%s%c",
                                        numberBuffer, line[j]);
                                    j++;
                                    foundNumber = true;
                                }
                                // If it's an argument, identify it
                                if(line[j] == '_'
                                    || (line[j] >= 'a'
                                        && line[j] <= 'z')) {
                                    // Allocates a buffer for the argument name
                                    char *argumentBuffer =
                                        malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
                                    argumentBuffer[0] = '\0';
                                    // Iterate until the character isn't valid
                                    while(line[j] == '_'
                                    || (line[j] >= 'a'
                                        && line[j] <= 'z')
                                    || (line[j] >= 'A'
                                        && line[j] <= 'Z')
                                    || (line[j] >= '0'
                                        && line[j] <= '9')) {
                                        // Appends character to buffer array
                                        sprintf(argumentBuffer, "%s%c",
                                            argumentBuffer, line[j]);
                                        j++;
                                    }
                                    // If it's a whitespace, ignore it
                                    while(line[j] == ' ') j++;
                                    // If the parameter ends properly
                                    if(line[j] == ']') {
                                        // Goes to next character
                                        j++;
                                        closedReference = true;
                                        // Checks if the argument was delcared previously
                                        int value = get_val_from_hashmap(arguments, 
                                            argumentBuffer);
                                        // If it exists, trigger "no argument" error
                                        if(value >= 0){
                                            if (*errorId == BUILD_ERR_NO_ERRORS) {
                                                *errorId = BUILD_ERR_REFERENCE_NO_ARGUMENT;
                                                break;
                                            }
                                            continue;
                                        } else if (*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_ARGUMENT_UNKNOWN;
                                            break;
                                        }
                                        // Triggers "no quantum measurement" error
                                        if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_REFERENCE_NO_QUBIT;
                                            break;
                                        }
                                    }
                                }
                                // If it's a label, identify it
                                if(line[j] >= 'A'
                                    && line[j] <= 'Z') {
                                    // Allocates a buffer for the argument name
                                    char *labelBuffer =
                                        malloc(sizeof(char) * MAX_LABEL_LENGTH);
                                    labelBuffer[0] = '\0';
                                    // Iterate until the character isn't valid
                                    while(line[j] == '_'
                                    || (line[j] >= 'a'
                                        && line[j] <= 'z')
                                    || (line[j] >= 'A'
                                        && line[j] <= 'Z')
                                    || (line[j] >= '0'
                                        && line[j] <= '9')) {
                                        // Appends character to buffer array
                                        sprintf(labelBuffer, "%s%c",
                                            labelBuffer, line[j]);
                                        j++;
                                    }
                                    // If it's a whitespace, ignore it
                                    while(line[j] == ' ') j++;
                                    // If the parameter ends properly
                                    if(line[j] == ']') {
                                        // Checks if the label exists
                                        int value = get_val_from_hashmap(labels, 
                                            labelBuffer);
                                        // If it exists, trigger "no label" error
                                        if(value >= 0) {
                                            if (*errorId == BUILD_ERR_NO_ERRORS) {
                                                *errorId = BUILD_ERR_REFERENCE_NO_LABEL;
                                                break;
                                            }
                                            continue;
                                        } else if (*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_LABEL_UNKNOWN;
                                            break;
                                        }
                                    }
                                }
                                // Checks if it's a classical register (reserved or not)
                                if(foundNumber
                                    && (line[j] == '!'
                                        || line[j] == '%')) {
                                    bool isReserved = line[j] == '%';
                                    // Ignores type character
                                    j++;
                                    // If it's a whitespace, ignore it
                                    while(line[j] == ' ') j++;
                                    // If the parameter ends properly
                                    if(line[j] == ']') {
                                        // Goes to next character
                                        j++;
                                        // Saves value into operation
                                        b.values[m] = atoi(numberBuffer);
                                        b.specials[m] = TAG_REFERENCE | isReserved * TAG_RESERVED;
                                        // Keeps track of necessary register count
                                        if(!isReserved && b.values[m] >= r->ctotal)
                                            r->ctotal = b.values[m] + 1;
                                        closedReference = true;
                                    }
                                // Checks if it's a quantum register
                                } else if(foundNumber
                                    && line[j] == '?') {
                                    // Ignores type character
                                    j++;
                                    // If it's a whitespace, ignore it
                                    while(line[j] == ' ') j++;
                                    // If the parameter ends properly
                                    if(line[j] == ']') {
                                        // Goes to next character
                                        j++;
                                        closedReference = true;
                                        // Triggers "no quantum measurement" error
                                        if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_REFERENCE_NO_QUBIT;
                                            break;
                                        }
                                    }
                                } else if(foundNumber
                                    && (line[j] == ']'
                                        || line[j] == ' ')) {
                                    // If it's a whitespace, ignore it
                                    while(line[j] == ' ') j++;
                                    // If the parameter ends properly
                                    if(line[j] == ']') {
                                        // Goes to next character
                                        j++;
                                        closedReference = true;
                                        // Triggers "no quantum measurement" error
                                        if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_REFERENCE_NO_VALUE;
                                            break;
                                        }
                                    }
                                } 
                                // If it's a whitespace, ignore it
                                while(!closedReference
                                    && line[j] == ' ') j++;
                                // If the finds closing character
                                if(line[j] == ']') {
                                    // Goes to next character
                                    j++;
                                    closedReference = true;
                                }
                                // If the reference ends properly
                                if(j >= sizeof(line)
                                        || line[j] == ' '
                                        || line[j] == '\0'
                                        || line[j] == '\n'
                                        || line[j] == '/') {
                                    // Checks if reference has been closed
                                    if(closedReference) {
                                        if(foundNumber) {
                                            // If task expected value, continues
                                            if(nyanTasks[l].parameters[m] ==
                                                TYPE_VAL) {
                                                // Goes to the next parameter
                                                m++;
                                                continue;
                                            } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                                *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
                                                break;
                                            }
                                        } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_REFERENCE_EMPTY;
                                            break;
                                        }
                                    } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                        *errorId = BUILD_ERR_REFERENCE_NO_SQUARE_BRACKET;
                                        break;
                                    }
                                }
                            }
                            // If it's a number, identify it
                            if(line[j] >= '0'
                                && line[j] <= '9'){
                                // Increments theoretical parameters count
                                o++;
                                // If the task is already filled with parameters, break
                                if(*errorId == BUILD_ERR_NO_ERRORS
                                    && o > n) {
                                    *errorId = BUILD_ERR_TASK_EXCESSIVE_PARAMS;
                                    break;
                                }
                                // Allocates a buffer for the number name
                                char *numberBuffer =
                                    malloc(sizeof(char) * MAX_NUM_LENGTH);
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
                                        || line[j] == ' '
                                        || line[j] == '/') {
                                        // Saves value into operation
                                        b.values[m] = atoi(numberBuffer);
                                        b.specials[m] = TAG_RESERVED * isReserved;
                                        // Keeps track of necessary register count
                                        if(!isReserved && b.values[m] >= r->ctotal)
                                            r->ctotal = b.values[m] + 1;
                                        // If task expected classical, continues
                                        if(nyanTasks[l].parameters[m] ==
                                            TYPE_BIT) {
                                            // Goes to the next parameter
                                            m++;
                                            continue;
                                        } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
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
                                        || line[j] == ' '
                                        || line[j] == '/') {
                                        // If task expected quantum, continues
                                        if(nyanTasks[l].parameters[m] ==
                                            TYPE_QUBIT) {
                                            // Saves value into operation
                                            b.values[m] = atoi(numberBuffer);
                                            b.specials[m] = TAG_NONE;
                                            // Keeps track of necessary qubit count
                                            if(b.values[m] >= r->qtotal) {
                                                r->qtotal = b.values[m] + 1;
                                            }
                                            // Goes to the next parameter
                                            m++;
                                            continue;
                                        } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                            *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
                                            break;
                                        }
                                    }
                                } else if(j >= sizeof(line)
                                        || line[j] == '\0'
                                        || line[j] == '\n'
                                        || line[j] == ' '
                                        || line[j] == '/') {
                                    // If task expected value, continues
                                    if(nyanTasks[l].parameters[m] ==
                                        TYPE_VAL) {
                                        // Saves value into operation
                                        b.values[m] = atoi(numberBuffer);
                                        b.specials[m] = TAG_NONE;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                                        *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
                                        break;
                                    }
                                }
                                break;
                            }
                            // If it's an argument, identify it
                            if(line[j] == '_'
                                || (line[j] >= 'a'
                                    && line[j] <= 'z')) {
                                // Increments theoretical parameters count
                                o++;
                                // If the task is already filled with parameters, break
                                if(*errorId == BUILD_ERR_NO_ERRORS
                                    && o > n) {
                                    *errorId = BUILD_ERR_TASK_EXCESSIVE_PARAMS;
                                    break;
                                } else if (*errorId == BUILD_ERR_NO_ERRORS
                                    && nyanTasks[l].parameters[m] != TYPE_VAL) {
                                    // If task didn't expected this value, continues
                                    *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
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
                                    && line[j] <= 'Z')
                                || (line[j] >= '0'
                                    && line[j] <= '9')) {
                                    // Appends character to buffer array
                                    sprintf(argumentBuffer, "%s%c",
                                        argumentBuffer, line[j]);
                                    j++;
                                }
                                // If the parameter ends properly
                                if(j >= sizeof(line)
                                    || line[j] == '\0'
                                    || line[j] == '\n'
                                    || line[j] == ' '
                                    || line[j] == '/') {
                                    // Checks if the argument was delcared previously
                                    int value = get_val_from_hashmap(arguments, 
                                        argumentBuffer);
                                    // If it exists, replace it with it's value
                                    if(value >= 0){
                                        // Saves value into operation
                                        b.values[m] = value;
                                        b.specials[m] = TAG_ARGUMENT;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if (*errorId == BUILD_ERR_NO_ERRORS) {
                                        *errorId = BUILD_ERR_ARGUMENT_UNKNOWN;
                                        break;
                                    }
                                }
                            }
                            // If it's a label, identify it
                            if(line[j] >= 'A'
                                && line[j] <= 'Z') {
                                // Increments theoretical parameters count
                                o++;
                                // If the task is already filled with parameters, break
                                if(*errorId == BUILD_ERR_NO_ERRORS
                                    && o > n) {
                                    *errorId = BUILD_ERR_TASK_EXCESSIVE_PARAMS;
                                    break;
                                } else if (*errorId == BUILD_ERR_NO_ERRORS
                                    && nyanTasks[l].parameters[m] != TYPE_LABEL) {
                                    // If task didn't expected this value, breaks
                                    *errorId = BUILD_ERR_TASK_UNEXPECTED_PARAM;
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
                                    && line[j] <= 'Z')
                                || (line[j] >= '0'
                                    && line[j] <= '9')) {
                                    // Appends character to buffer array
                                    sprintf(labelBuffer, "%s%c",
                                        labelBuffer, line[j]);
                                    j++;
                                }
                                // If the parameter ends properly
                                if(j >= sizeof(line)
                                    || line[j] == '\0'
                                    || line[j] == '\n'
                                    || line[j] == ' '
                                    || line[j] == '/') {
                                    // Checks if the label exists
                                    int value = get_val_from_hashmap(labels, 
                                        labelBuffer);
                                    // If it exists, replace it with it's value
                                    if(value >= 0) {
                                        // Saves value into operation
                                        b.values[m] = value;
                                        b.specials[m] = TAG_NONE;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    } else if (*errorId == BUILD_ERR_NO_ERRORS) {
                                        *errorId = BUILD_ERR_LABEL_UNKNOWN;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        if(o == n) {
                            put_val_on_list(r->algorithm, b);
                            continue;
                        } else if(*errorId == BUILD_ERR_NO_ERRORS){
                            *errorId = BUILD_ERR_TASK_MISSING_PARAMS;
                            break;
                        }
                    } else {
                        // Checks if programmer actually meant to reference an argument
                        int index = get_val_from_hashmap(arguments,
                            taskBuffer);
                        
                        if(index >= 0) {
                            if(*errorId == BUILD_ERR_NO_ERRORS) {
                                *errorId = BUILD_ERR_ARGUMENT_LOOSE_REFERENCE;
                                break;
                            }
                        } else if(*errorId == BUILD_ERR_NO_ERRORS) {
                            *errorId = BUILD_ERR_TASK_UNKNOWN;
                            break;
                        }
                    }
                }

                // Identifies declaring labels
                if(line[j] >= 'A'
                    && line[j] <= 'Z') {
                    // If line has already another function, throw an error
                    if(*errorId == BUILD_ERR_NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        *errorId = BUILD_ERR_LABEL_ILLEGAL;
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
                            p
                        );
                        if(*errorId == BUILD_ERR_NO_ERRORS
                            && !success) {
                            *errorId = BUILD_ERR_LABEL_DUPLICATED;
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
                    if(*errorId == BUILD_ERR_NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        *errorId = BUILD_ERR_ARGUMENT_ILLEGAL;
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
                                if(*errorId == BUILD_ERR_NO_ERRORS
                                    && !success) {
                                    *errorId = BUILD_ERR_ARGUMENT_DUPLICATED;
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
                            } else if (*errorId == BUILD_ERR_NO_ERRORS) {
                                *errorId = BUILD_ERR_ARGUMENT_UNNECESSARY;
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
                            if(*errorId == BUILD_ERR_NO_ERRORS
                                && foundArgument
                                && foundWhitespace) {
                                *errorId = BUILD_ERR_ARGUMENT_NO_COMMA;
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
                        if(*errorId == BUILD_ERR_NO_ERRORS
                            && !closedDeclaration) {
                            *errorId = BUILD_ERR_ARGUMENT_NO_ANGLE_BRACKET;
                            break;
                        }
                    }
                    continue;
                }
                
                // Identifies character as loose
                if(*errorId == BUILD_ERR_NO_ERRORS) {
                    printf("a: %c\n", line[j]);
                    *errorId = BUILD_ERR_GENERAL_ILLEGAL_CHAR;
                }
                break;
            }
        }
            
    }
}

// Loads a .nya script
qscript load_script(char *path, bool echo) {
    // Build Error
    nyanBuildError errorId = BUILD_ERR_NO_ERRORS;
    // Result
    qscript r = { new_list(), 0, 0 };

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
        lex_script(f, &r, &errorId, labels, params, true);
        // Rewinds the file
        rewind(f);
        // Actual building
        lex_script(f, &r, &errorId, labels, params, false);
    } else {
        errorId = BUILD_ERR_GENERAL_NOT_NYA;
    }
    
    fclose(f);
    if(errorId < 0) {      
        if(echo) {
            printf("nyancat: build succeeded c:\n");
            int i = 0;
            listnode *temp = r.algorithm->first;
            while(temp && i < r.algorithm->size) {
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
                    printf("\t%i (%8s)", temp->val.values[k], name);
                }
                printf("\n");
                temp = temp->next;
                i++;
            }
        }
    } else {
        print_build_error(errorId);
    }
    return r;
}
#endif