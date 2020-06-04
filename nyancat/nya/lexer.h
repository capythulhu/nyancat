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

#include "commands.h"
#include "../utils/hashmap.h"

#define MAX_ERROR_LENGTH    1<<8
#define MAX_LINE_LENGTH     1<<8
#define MAX_PARAM_LENGTH    1<<7
#define MAX_PARAMS          1<<6
#define MAX_LABEL_LENGTH    1<<7
#define MAX_LABELS          1<<6

typedef struct _algorithm {
    command     *cmds;
    unsigned    paramc;
} algorithm;

algorithm load_script(char *path);

typedef enum _nyanError {
    NO_ERRORS = -1,
    GENERAL_ILLEGAL_CHAR,
    PARAM_ILLEGAL,
    PARAM_NO_COMMA,
    PARAM_UNFINISHED,
    PARAM_UNNECESSARY,
    PARAM_NO_ANGLE_BRACKET,
    LABEL_ILLEGAL,
    OPERATION_ILLEGAL
} nyanError;

typedef enum _nyanLine {
    LINE_UNDEFINED = -1,
    LINE_PARAM,
    LINE_LABEL,
    LINE_OPERATION,
} _nyanLine;

// Lexes a .nya script
algorithm load_script(char *path) {
    // Compilation Error
    int errorId = NO_ERRORS;
    // General variables
    algorithm a = { NULL, 0 };
    FILE *f;
    char line[MAX_LINE_LENGTH];

    // Parameters
    hashmap *params = new_hashmap();
    // Labels
    hashmap *labels = new_hashmap();
    
    char *finalPath = malloc(sizeof(char) * (strlen(path) + strlen(".nya")));
    sprintf(finalPath, "%s%s", path, ".nya");
    f = fopen(finalPath, "r");

    if(f) {
        while(fgets(line, MAX_LINE_LENGTH, f)) { 
            // Char counter
            int j = 0;
            // Line definition
            int k = LINE_UNDEFINED;

            while(j < sizeof(line)
                && line[j] != '\0'
                && line[j] != '\n') {
                
                // Checks if the character is a whitespace
                if(line[j] == ' ') {
                    j++;
                    continue;
                }
                
                // Checks if the character is an in-line comment
                if(line[j] == '/'
                    && line[j + 1] == '/') {
                    break;
                }

                // Identifies operations
                if(line[j] >= 'a'
                    && line[j] <= 'z') {
                    if(errorId == NO_ERRORS
                            && k != LINE_UNDEFINED) {
                        errorId = OPERATION_ILLEGAL;
                        break;
                    }
                    k = LINE_OPERATION;
                    // TODO
                    j++;
                    continue;
                }

                // Identifies declaring labels
                if(line[j] == ':') {
                    if(errorId == NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        errorId = LABEL_ILLEGAL;
                        break;
                    }
                    k = LINE_LABEL;
                    // If it matches ^:[a-zA-Z], enters the label loop
                    if((line[j + 1] >= 'a'
                            && line[j + 1] <= 'z')
                        || (line[j + 1] >= 'A'
                            && line[j + 1] <= 'Z')) {
                        // Allocates a buffer for the name
                        char *labelBuffer = malloc(sizeof(char) * MAX_LABEL_LENGTH);
                        labelBuffer[0] = '\0';
                        // Ignores ':'
                        j++;
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
                        put_val_on_hashmap(
                            labels,
                            labelBuffer,
                            labels->size
                        );
                        // Frees buffer
                        free(labelBuffer);
                        continue;
                    }
                }

                // Identifies opening params
                if(line[j] == '<') {
                    if(errorId == NO_ERRORS
                        && k != LINE_UNDEFINED) {
                        errorId = PARAM_ILLEGAL;
                        break;
                    }
                    k = LINE_PARAM;
                    // If the parameter declaration has been closed already
                    bool closedDeclaration = false;
                    // If any parameter has been found
                    bool foundParam = false;
                    // If a whitespace has been found
                    bool foundWhitespace = false;
                    // Ignores opening character
                    j++;
                    // Allocates a buffer for the name
                    char *paramBuffer = malloc(sizeof(char) * MAX_PARAM_LENGTH);
                    paramBuffer[0] = '\0';
                    do {
                        if(line[j] == ' ') {
                            foundWhitespace = true;
                            j++;
                        }
                        // If it's a valid separator, records param
                        if(line[j] == ','
                            || line[j] == '>') {
                            if(foundParam) {
                                put_val_on_hashmap(
                                    params,
                                    paramBuffer,
                                    params->size
                                );
                                // Finishes param declaration
                                foundParam = false;
                                // Closes declaration if found closing bracket
                                if(line[j] == '>')
                                    closedDeclaration = true;
                                // Ignores separator character
                                j++;
                                // Resets buffer
                                paramBuffer[0] = '\0';
                            } else if (errorId == NO_ERRORS) {
                                errorId = PARAM_UNNECESSARY;
                                break;
                            }
                        }
                        // If it matches ^:[a-zA-Z0-9], enters the param loop
                        while(!closedDeclaration
                            && (line[j] >= 'a'
                                && line[j] <= 'z')
                            || (line[j] >= 'A'
                                && line[j] <= 'Z')
                            || (line[j] >= '0'
                                && line[j] <= '9')) {
                            // Report error if last param is unfinished
                            if(errorId == NO_ERRORS
                                && foundParam
                                && foundWhitespace) {
                                errorId = PARAM_NO_COMMA;
                                break;
                            }
                            // Current character is not a whitespace
                            foundWhitespace = false;
                            // Parameter declaration found
                            if(!foundParam) foundParam = true;
                            // Appends character to buffer array
                            sprintf(paramBuffer, "%s%c", paramBuffer, line[j]);
                            // Goes to next character
                            j++;
                        }
                    } while(line[j] == ' '
                        || line[j] == ','
                        || line[j] == '>');
                    // Frees buffer
                    free(paramBuffer);
                    // If there is an unfinished param, throw error
                    if(errorId == NO_ERRORS
                        && foundParam) 
                        errorId = PARAM_UNFINISHED;
                    // If there's no closing bracket, throw error
                    if(errorId == NO_ERRORS
                        && !closedDeclaration) {
                        errorId = PARAM_NO_ANGLE_BRACKET;
                        break;
                    }
                    continue;
                }
                if(errorId == NO_ERRORS) {
                    printf("a: %c\n", line[j]);
                    errorId = GENERAL_ILLEGAL_CHAR;
                }
                break;
            }
        }
    }

    fclose(f);
    if(errorId < 0) {
        printf("params: %i\n", params->size);
        printf("labels: %i\n", labels->size);
    } else {
        char *errorMsg;
        switch(errorId) {
            case GENERAL_ILLEGAL_CHAR:
                errorMsg = "Illegal loose character.";
                break;
            case PARAM_ILLEGAL:
                errorMsg = "Illegal parameter declaration.";
                break;
            case PARAM_NO_COMMA:
                errorMsg = "No comma on parameter declaration.";
                break;
            case PARAM_UNFINISHED:
                errorMsg = "Unfinished parameter declaration.";
                break;
            case PARAM_UNNECESSARY:
                errorMsg = "Unnecessary statement in parameter declaration.";
                break;   
            case PARAM_NO_ANGLE_BRACKET:
                errorMsg = "Missing angle bracket.";
                break;    
            case LABEL_ILLEGAL:
                errorMsg = "Illegal label declaration.";
                break;
            case OPERATION_ILLEGAL:
                errorMsg = "Illegal operation call.";
                break;
            default:
                errorMsg = "Unknown error.";
                break;
        }
        printf("error: %s\n", errorMsg);
    }
    return a;
}
#endif