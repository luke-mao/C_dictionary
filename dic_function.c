#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dic_function.h"


// some constants
#define P 53
#define M (long) 170846407
#define UPSIZE_THRESHOLD 0.7
#define DOWNSIZE_THRESHOLD 0.2


// find the hash of the key
long hash_index(char* string, int p, long m);

Item* insert_item(char* key, char* value);
void update_item(Item* item_p, char* value);

// when load factor above threshold, increase the size
void upsize(Dictionary d);
// when load factor below threshold, decrease the size
void downsize(Dictionary d);



Dictionary create(long size){

    // malloc the largest structure: Dictionary
    Dictionary d;
    d = (Dictionary) malloc (sizeof(dic));
    if (d==NULL){
        printf("Memory allocation fail: create\n");
        exit(EXIT_FAILURE);
    }
    
    d->size = (long) size;
    d->count = (long) 0;

    // malloc the second largest structure: bucket_array
    d->bucket_array = (Item**) malloc (size * sizeof(Item*));
    if(d->bucket_array == NULL){
        printf("Memory allocation fail: create 2\n");
        exit(EXIT_FAILURE);
    } 
    
    // NULL all item pointer
    for (long i = 0; i < d->size; i++){
        *(d->bucket_array + i) = NULL;
    }
    
    return d;
}


Dictionary destroy(Dictionary d){
    // need to free from the most inside structure
    for (long i = 0; i < d->size; i++){
        if (*(d->bucket_array+i) != NULL){
            free((*(d->bucket_array+i))->key);
            free((*(d->bucket_array+i))->value);
            free(*(d->bucket_array+i));
        }
    }
    free(d->bucket_array);
    d->bucket_array = NULL;
    
    free(d);
    d = NULL;
    
    return d;
}


// note: insert serves as an update too. 
void insert(Dictionary d, char* key, char* value){

    // first check if we need to upsize
    // downsize only occurs during delete, so no worry at insert stage
    if (((float)d->count+1) / (float)d->size > UPSIZE_THRESHOLD){
        upsize(d);
    }
    
    // insert a value into dictionary
    // do the hash first
    long key_hash = hash_index(key, P, M);
    key_hash %= d->size; // d->size causes the segmentation fault
    
    // if the position is allocated already, check the key is same or not
    // if the key is the same, then update "value"
    // if not, either: find the next empty slot, or find the same key

    // if it is null, simply exist
    // if it is not null, check if key is the same, if same, exist
    // if the key not same, then roll to find either an empty position or a same key
    if (*(d->bucket_array + key_hash) != NULL){
        if (strcmp((*(d->bucket_array + key_hash))->key, key) != 0){
            while (true){
                key_hash++;
                key_hash %= d->size;
                if(*(d->bucket_array + key_hash) == NULL 
                    || strcmp((*(d->bucket_array + key_hash))->value, value) == 0){
                    break;
                }
            }
        }
    }

    // now use the key hash to check if the position is null or not
    // if null, malloc, if not null, means same key, update the value
    if (*(d->bucket_array + key_hash) == NULL){
        // now input the key and value
        // use a function
        *(d->bucket_array + key_hash) = insert_item(key, value);
        d->count++;
    }
    else{
        // same key, need to update the value
        if (strcmp((*(d->bucket_array + key_hash))->value, value) != 0){
            // update the item
            update_item(*(d->bucket_array + key_hash), value);
        }  
    }

    return;
}


/* need to fix the delete: 
if one item is removed, it may be on the collision chain
so that it may break the chain. need to modify the delete
probably simply make the value as null*/
void delete(Dictionary d, char* key){
    // if the key is not there, then do nothing
    // if the key is there, find it and then null this bucket
    long key_hash = hash_index(key, P, M);
    key_hash %= d->size;

    if (*(d->bucket_array+key_hash) == NULL){
        printf("No key-value pair matches key = [%s]\n", key);
        return;
    }
    else{
        //find along the array until meet the first null or find the same key
        while (true){
            if (strcmp( (*(d->bucket_array+key_hash))->key, key) == 0){
                break;
            }
            if (*(d->bucket_array+key_hash)==NULL){
                break;
            }

            key_hash++;
            key_hash %= d->size;
        }

        if (*(d->bucket_array+key_hash) == NULL){
            printf("No key-value pair matches key = [%s]\n", key);
            return;
        }
        else{
            free((*(d->bucket_array+key_hash))->key);
            (*(d->bucket_array+key_hash))->key = NULL;

            free((*(d->bucket_array+key_hash))->value);
            (*(d->bucket_array+key_hash))->value = NULL;

            free(*(d->bucket_array+key_hash));
            *(d->bucket_array + key_hash) = NULL; 
        }
    }
    
    // after delete, reduce the counter
    d->count--;

    // at final, check if we need to downsize
    // if the dic size > 30, and the load factor < threshold
    if ( (float)d->count / (float)d->size < DOWNSIZE_THRESHOLD  
            && d->size > START_SIZE){
        downsize(d);
        printf("Dictionary downsize.\n");
        status(d);
        printf("\n");
    }

    return;
}


char* search(const Dictionary d, char*key){

    char* error_msg;
    error_msg = (char*) malloc (50*sizeof(char));
    sprintf(error_msg, "Error: no key [%s]\n", key);

    long key_hash = hash_index(key, P, M);
    key_hash %= d->size;


    if (*(d->bucket_array+key_hash) == NULL){
        return error_msg;
    }
    else{
        if (strcmp( (*(d->bucket_array+key_hash))->key, key) == 0){
            // better to copy the values, in case for further use
            char* ret_value;
            ret_value = (char*) malloc(strlen((*(d->bucket_array+key_hash))->value)*sizeof(char));
            strcpy(ret_value, (*(d->bucket_array+key_hash))->value);
            return ret_value;
        }
        else{
            while (true){
                key_hash++;
                key_hash %= d->size;
                if (*(d->bucket_array+key_hash) == NULL){
                    return error_msg;
                }
                else if (strcmp( (*(d->bucket_array+key_hash))->key, key) == 0){
                    // better to copy the values, in case for further use
                    char* ret_value;
                    ret_value = (char*) malloc(strlen((*(d->bucket_array+key_hash))->value)*sizeof(char));
                    strcpy(ret_value, (*(d->bucket_array+key_hash))->value);
                    return ret_value;
                }
            }
        }
    }
}


// return some status information 
void status(const Dictionary d){

    printf("Dictionary status:\n");
    printf("Size = %ld   Item number = %ld   ",
            d->size, d->count);
    
    if (d->count != 0){
        printf("Load factor = %.0f%%", (float)(d->count) / (float)d->size * 100.0);
    }
    printf("\n\n");
    return;
}


// this function prints out all the keys 
void view_pairs(const Dictionary d){

    printf("Dictionary key-value pairs:\n");

    for (int i = 0; i < d->size; i++){
        if (*(d->bucket_array + i) != NULL){
            printf("[%s] -> [%s]\n", 
                (*(d->bucket_array+i))->key, (*(d->bucket_array+i))->value);
        }
    }
    
    return;
}


/////////////////////////////////////////////////////////////////////////////
// find the hash of the key
long hash_index(char* string, int p, long m){

    int size = strlen(string);
    long value = 0;
    long power = 1;

    for (int i = 0; i < size; i++){
        value += ((long)string[i] * power * (i+1)) % m;
        power *= (long) p;
    }
    
    return value;
}


// upsize function: increase volume by double
void upsize(Dictionary d){

    printf("Dictionary upsizing...\n");
    printf("Original status:");
    status(d);

    Dictionary dnew;
    dnew = create((long) (d->size * 2));
    dnew->count = d->count; // the number in dic is not changed

    //now need to copy all values to here
    // scan through d, find a key-value, then re-calculate the hash and allocate
    for (long i = 0; i < d->size; i++){
        if (*(d->bucket_array + i) != NULL){
            long key_hash = hash_index(
                (*(d->bucket_array+i))->key, P, M
            );
            key_hash %= dnew->size;
            //check if the location is filled
            while (*(dnew->bucket_array+key_hash) != NULL){
                // we can assume that no two identical keys
                // as otherwise it will simply update
                // so here we just find a null is enough
                key_hash++;
                key_hash %= dnew->size;
            }

            // now put key and value in. The structure can be copied, so make it easy
            *(dnew->bucket_array+key_hash) = *(d->bucket_array+i);
        }
    }

    // 书page 454 说现在允许把一个结构复制给另一个结构，可以试试
    *d = *dnew;
    free(dnew);

    printf("New status:\n");
    status(d);
    return;
}


// downsize function: called in the "delete" function
// the downsize function reduces to max(d->size / 2, START_SIZE)
void downsize(Dictionary d){

    printf("Dictionary downsizing...\n");
    printf("Original status:");
    status(d);

    Dictionary dnew;
    if ((d->size / 2) > START_SIZE){
        dnew = create((long) (d->size / 2));
    }
    else{
        dnew = create((long) START_SIZE);
    }

    dnew->count = d->count; // the number in dic is not changed

    //now need to copy all values to here
    // scan through d, find a key-value, then re-calculate the hash and allocate
    for (long i = 0; i < d->size; i++){
        if (*(d->bucket_array + i) != NULL){
            long key_hash = hash_index(
                (*(d->bucket_array+i))->key, P, M
            );
            key_hash %= dnew->size;
            //check if the location is filled
            while (*(dnew->bucket_array+key_hash) != NULL){
                // we can assume that no two identical keys
                // as otherwise it will simply update
                // so here we just find a null is enough
                key_hash++;
                key_hash %= dnew->size;
            }

            // now put key and value in
            *(dnew->bucket_array+key_hash) = *(d->bucket_array+i);
        }
    }

    // 书page 454 说现在允许把一个结构复制给另一个结构，可以试试
    *d = *dnew;
    free(dnew);

    printf("New status:\n");
    status(d);

    return;
}


// insert item function: malloc and strcpy
Item* insert_item(char* key, char* value){
    Item* item_p;
    item_p = (Item*) malloc (1*sizeof(Item));
    
    if (item_p == NULL){
        printf("Memory allocation fail: insert\n");
        exit(EXIT_FAILURE);
    }

    // copy the key and value into it, need to malloc first
    item_p->key = (char*) malloc (strlen(key)*sizeof(char));
    if (item_p->key == NULL){
        printf("Memory allocation fail: insert 2\n");
        exit(EXIT_FAILURE);
    }
    strcpy(item_p->key, key);

    item_p->value = (char*) malloc (strlen(value)*sizeof(char));
    if (item_p->value == NULL){
        printf("Memory allocation fail: insert 3\n");
        exit(EXIT_FAILURE);
    }
    strcpy(item_p->value, value);
    
    return item_p;
}


// update item
void update_item(Item* item_p, char* value){
    item_p = realloc(
        item_p, strlen(value)*sizeof(char)
    );

    if (item_p->value == NULL){
        printf("Memory allocation fail: insert\n");
        exit(EXIT_FAILURE);
    }

    strcpy(item_p->value, value);
    return;
}