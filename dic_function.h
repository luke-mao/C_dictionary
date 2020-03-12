#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define START_SIZE 30

typedef struct{
    char* key;
    char* value;
} Item;

typedef struct{
    long size;
    long count;
    Item** bucket_array; // item pointer array
} dic;

typedef dic* Dictionary;

// basic dic functions: create, destroy, insert, update, delete
Dictionary create(long size);
Dictionary destroy(Dictionary d);
void insert(Dictionary d, char* key, char* value); // no update, simply: insert = update
void delete(Dictionary d, char* key);
char* search(const Dictionary d, char* key); // return NULL if not find the key-value pair, else just return the value
void status(const Dictionary d);
void view_pairs(const Dictionary d);

