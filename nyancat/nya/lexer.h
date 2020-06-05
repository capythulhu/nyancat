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
#include "commands.h"
#include "../utils/hashmap.h"

#define MAX_ERROR_LENGTH    1<<8
#define MAX_LINE_LENGTH     1<<8
#define MAX_ARGUMENT_LENGTH 1<<7
#define MAX_LABEL_LENGTH    1<<7
#define MAX_PARAM_LENGTH    1<<6

typedef struct _algorithm {
    command     *cmds;
    unsigned    paramc;
} algorithm;

algorithm load_script(char *path);

typedef enum _nyanError {
    NO_ERRORS = -1,
    GENERAL_NOT_NYA,
    GENERAL_ILLEGAL_CHAR,
    ARGUMENT_ILLEGAL,
    ARGUMENT_DUPLICATED,
    ARGUMENT_NO_COMMA,
    ARGUMENT_UNNECESSARY,
    ARGUMENT_NO_ANGLE_BRACKET,
    LABEL_ILLEGAL,
    LABEL_DUPLICATED,
    TASK_ILLEGAL,
    TASK_UNKNOWN
} nyanError;

typedef enum _nyanLine {
    LINE_UNDEFINED = -1,
    LINE_ARGUMENT,
    LINE_LABEL,
    LINE_TASK,
} _nyanLine;

// Lexes a .nya script
void lex_script(FILE *f, int *errorId, hashmap *labels, hashmap *arguments, bool preCompile) {
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
                    // Doesn't declare tasks on pre compiling
                    if(preCompile) break;
                    // If line has already another function, throw an error
                    if(*errorId == NO_ERRORS
                            && k != LINE_UNDEFINED) {
                        *errorId = TASK_ILLEGAL;
                        break;
                    }
                    // Makes line a task line
                    k = LINE_TASK;
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
                        // Parameter counter
                        int m = 0;
                        // Parameter types
                        nyanParam parameters[MAX_PARAM_LENGTH] = { TYPE_VOID };
                        // Loops through line
                        while(j < sizeof(line)
                            && line[j] != '\0'
                            && line[j] != '\n') {
                            // If it's a whitespace, ignore it
                            while(line[j] == ' ') j++;
                            // If it's a number, identify it
                            if(line[j] >= '0'
                                && line[j] <= '9'){
                                // Iterate until the character isn't a number
                                while(line[j] >= '0'
                                    && line[j] <= '9') j++;
                                // Checks it's a classical register (reserved or not)
                                if(line[j] == '!'
                                    || line[j] == '%') {
                                        // Ignores type character
                                        j++;
                                        // If the parameter ends properly
                                        if(j >= sizeof(line)
                                            || line[j] != '\0'
                                            || line[j] != '\n'
                                            || line[j] != ' ') {
                                            // Sets the parameter type to classical
                                            parameters[m] = TYPE_BIT;
                                            // Goes to the next parameter
                                            m++;
                                            continue;
                                        }
                                // Checks if it's a quantum register
                                } else if(line[j] == '?') {
                                    // Ignores type character
                                    j++;
                                    // If the parameter ends properly
                                    if(j >= sizeof(line)
                                        || line[j] != '\0'
                                        || line[j] != '\n'
                                        || line[j] != ' ') {
                                        // Sets the parameter type to quantum
                                        parameters[m] = TYPE_QUBIT;
                                        // Goes to the next parameter
                                        m++;
                                        continue;
                                    }
                                }
                            }
                            if(line[j] >= 'a'
                                && line[j] <= 'z'){
                                parameters[m] = TYPE_VAL;
                            }
                            if(line[j] >= 'A'
                                && line[j] <= 'Z'){
                                parameters[m] = TYPE_LABEL;
                            }
                        }
                    } else if(*errorId == NO_ERRORS) {
                        *errorId = TASK_UNKNOWN;
                        break;
                    }
                }

                // Identifies declaring labels
                if(line[j] >= 'A'
                    && line[j] <= 'Z') {
                    // Doesn't declare labels on compiling
                    if(!preCompile) break;
                    // If line has already another function, throw an error
                    if(*errorId == NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        *errorId = LABEL_ILLEGAL;
                        break;
                    }
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
                        || line[j] == '\n') {
                        bool success = put_val_on_hashmap(
                            labels,
                            labelBuffer,
                            labels->size
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
                    // Doesn't declare arguments on pre compiling
                    if(preCompile) break;
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
                    } while(line[j] == ' '
                        || line[j] == ','
                        || line[j] == '>');
                    // Frees buffer
                    free(argumentBuffer);
                    if(line[j] == '\0'
                        || line[j] == '\n') {
                        // If there's no closing bracket, throw error
                        if(*errorId == NO_ERRORS
                            && !closedDeclaration) {
                            *errorId = ARGUMENT_NO_ANGLE_BRACKET;
                            break;
                        }
                        continue;
                    }
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
algorithm load_script(char *path) {
    // Compilation Error
    int errorId = NO_ERRORS;
    // General variables
    algorithm a = { NULL, 0 };
    FILE *f;

    // Parameters
    hashmap *params = new_hashmap();
    // Labels
    hashmap *labels = new_hashmap();
    
    char *finalPath = malloc(sizeof(char) * (strlen(path) + strlen(".nya")));
    sprintf(finalPath, "%s%s", path, ".nya");
    f = fopen(finalPath, "r");

    if(f) {
        // Pre Compiling (finding labels)
        lex_script(f, &errorId, labels, params, true);
        // Rewinds the file
        rewind(f);
        // Actual compiling
        lex_script(f, &errorId, labels, params, false);
    } else {
        errorId = GENERAL_NOT_NYA;
    }
    
    fclose(f);
    if(errorId < 0) {
        int l;
        hashnode *temp;
        
        printf("nyancat: lexing succeeded.\n");
        printf("%i params:\n", params->size);
        
        l = 0;
        temp = params->first;
        while(temp) {
            printf("\t%s : %i\n", temp->key, l++);
            temp = temp->next;
        }

        printf("%i labels:\n", labels->size);

        l = 0;
        temp = labels->first;
        while(temp) {
            printf("\t%s : %i\n", temp->key, l++);
            temp = temp->next;
        }
    } else {
        char *errorMsg;
        switch(errorId) {
            case GENERAL_NOT_NYA:
                errorMsg = "File does not exist or is not a .nya script.";
                break;
            case GENERAL_ILLEGAL_CHAR:
                errorMsg = "Illegal loose character.";
                break;
            case ARGUMENT_ILLEGAL:
                errorMsg = "Illegal argument declaration.";
                break;
            case ARGUMENT_DUPLICATED:
                errorMsg = "Non-unique argument name declaration.";
                break;
            case ARGUMENT_NO_COMMA:
                errorMsg = "No comma on argument declaration.";
                break;
            case ARGUMENT_UNNECESSARY:
                errorMsg = "Unnecessary statement in argument declaration.";
                break;   
            case ARGUMENT_NO_ANGLE_BRACKET:
                errorMsg = "Missing angle bracket.";
                break;    
            case LABEL_ILLEGAL:
                errorMsg = "Illegal label declaration.";
                break;
            case LABEL_DUPLICATED:
                errorMsg = "Non-unique label name declaration.";
                break;
            case TASK_ILLEGAL:
                errorMsg = "Illegal task call.";
                break;
            case TASK_UNKNOWN:
                errorMsg = "Unknown task call.";
                break;
            default:
                errorMsg = "Unknown error.";
                break;
        }
        printf("nyancat: lexing failed.\n");
        printf("error: %s\n", errorMsg);
    }
    return a;
}
#endif