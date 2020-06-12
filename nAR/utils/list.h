#ifndef LIST_H
#define LIST_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDBOOL_H
#define STDBOOL_H
#include <stdbool.h>
#endif

#include "../nya/operations.h"

// List node
typedef struct _listnode {
    nyanOperation val;
    struct _listnode *next;
} listnode;

// List
typedef struct _list{
    unsigned size;
    struct _listnode *first;
} list;

list *new_list(void);
listnode *new_listnode(nyanOperation val);
bool put_val_on_list(list *l, nyanOperation val);
nyanOperation get_val_from_list(list *l, unsigned index);

// New list
list *new_list(void) {
    list *output = malloc(sizeof(list));
    output->size = 0;
    output->first = NULL;
    return output;
}

// New list node
listnode *new_listnode(nyanOperation val) {
    listnode *output = malloc(sizeof(listnode));
    output->val = val;
    output->next = NULL;
    return output;
}

// Inserts node into list
bool put_val_on_list(list *l, nyanOperation val) {
    if(!l) return false;
    
    listnode *n = malloc(sizeof(listnode));
    n->val = val;
    n->next = NULL;

    listnode *temp = l->first;
    if(l->size <= 0 || !temp) {
        l->first = n;
        l->size = 1;
        return true;
    }

    while(temp->next) temp = temp->next;
    temp->next = n;
    l->size++;
    
    return true;
}

// Gets a value from the index associated with it
nyanOperation get_val_from_list(list *l, unsigned index) {
    if(!l) return (nyanOperation){-1, 0, false};

    int i = 0;
    listnode *temp = l->first;
    while(temp) {
        if(i == index) return temp->val;
        i++;
        temp = temp->next;
    }
    return (nyanOperation){-1, 0, false};
}
#endif