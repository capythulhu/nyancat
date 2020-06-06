#ifndef NYANCAT_UTILS_HASHMAP_H
#define NYANCAT_UTILS_HASHMAP_H

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

// Hashmap node
typedef struct _hashnode {
    char *key;
    int val;
    struct _hashnode *next;
} hashnode;
// Hashmap
typedef struct _hashmap{
    unsigned size;
    hashnode *first;
} hashmap;

#define MAX_KEY_LENGTH 1<<8

hashmap *new_hashmap(void);
hashnode *new_hashnode(char *key, int val);
bool put_hashnode(hashmap *h, hashnode *n);
bool rem_key_from_hashmap(hashmap *h, char *key);
int get_val_from_hashmap(hashmap *h, char *key);
char *get_key_from_hashmap(hashmap *h, int val);
hashnode *get_hashnode_from_hashmap(hashmap *h, char *key);
bool put_val_on_hashmap(hashmap *h, char *key, int val);

// New hashmap
hashmap *new_hashmap(void) {
    hashmap *output = malloc(sizeof(hashmap));
    output->size = 0;
    output->first = NULL;
    return output;
}

// New hashmap node
hashnode *new_hashnode(char *key, int val) {
    hashnode *output = malloc(sizeof(hashnode));
    output->key = key;
    output->val = val;
    output->next = NULL;
    return output;
}

// Inserts hashmap node
bool put_hashnode(hashmap *h, hashnode *n) {
    if(!n || !h) return false;

    hashnode *temp = h->first;
    if(h->size <= 0 || !temp) {
        h->first = n;
        h->size = 1;
        return true;
    }

    while(temp->next) {
        if(strcmp(temp->key, n->key) == 0) return false;
        temp = temp->next;
    }
    temp->next = n;
    h->size++;

    return true;
}

// Removes node from hashmap
bool rem_key_from_hashmap(hashmap *h, char *key) {
    if(!h) return false;

    hashnode *temp = h->first;
    if(h->size <= 0 || !temp) return false;

    if(strcmp(temp->key, key) == 0) {
        h->first = temp->next;
        free(temp);
        h->size--;
        return true;
    }
    if(!temp->next) return false;
    
    hashnode *prev;
    do {
        prev = temp;
        temp = temp->next;
        if(strcmp(temp->key, key) == 0) {
            prev->next = temp->next;
            free(temp);
            h->size--;
            return true;
        }
    } while(temp->next);
    return false;
}

// Gets a value from the key associated with it
int get_val_from_hashmap(hashmap *h, char *key) {
    if(!h) return 0;

    hashnode *temp = h->first;
    if(!temp) return 0;

    while(temp->next) {
        if(strcmp(temp->key, key) == 0) return temp->val;
        temp = temp->next;
    }
    
    if(strcmp(temp->key, key) == 0) return temp->val;
    return -1;
}

// Gets the first key associated with a value
char *get_key_from_hashmap(hashmap *h, int val) {
    if(!h) return NULL;

    hashnode *temp = h->first;
    if(!temp) return NULL;

    while(temp->next) {
        if(temp->val == val) return temp->key;
        temp = temp->next;
    }
    
    if(temp->val == val) return temp->key;
    return NULL;
}

// Inserts a value on a hashmap node, and then binds it to a hashmap
bool put_val_on_hashmap(hashmap *h, char *key, int val) {
    if(!h) return false;
    
    hashnode *temp = h->first;

    while(temp) {
        if(strcmp(temp->key, key) == 0) return false;
        temp = temp->next;
    }

    hashnode *n = malloc(sizeof(hashnode));
    n->key = malloc(sizeof(char) * MAX_KEY_LENGTH);
    n->key[0] = '\0';
    strcpy(n->key, key);
    n->val = val;
    n->next = NULL;

    temp = h->first;
    if(h->size <= 0 || !temp) {
        h->first = n;
        h->size = 1;
        return true;
    }
    while(temp->next) temp = temp->next;
    temp->next = n;
    h->size++;
    
    return true;
}
#endif