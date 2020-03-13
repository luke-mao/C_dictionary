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
void update_item(char* item_p_value, char* value);

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


// destroy the dictionary, free everything
Dictionary destroy(Dictionary d){
    // need to free from the most inside structure
    for (long i = 0; i < d->size; i++){
        if (*(d->bucket_array+i) != NULL){

            free((*(d->bucket_array+i))->key);
            (*(d->bucket_array+i))->key = NULL;

            if ((*(d->bucket_array+i))->value != NULL){
                free((*(d->bucket_array+i))->value);
                (*(d->bucket_array+i))->value = NULL;
            }
            
            free(*(d->bucket_array+i));
            *(d->bucket_array+i) = NULL;
        }
    }

    free(d->bucket_array);
    d->bucket_array = NULL;
    
    free(d);
    d = NULL;
    
    return d;
}


// note: insert serves as an update too. 
void insert_to_dic(Dictionary d, char* key, char* value){

    // first check if we need to upsize
    // downsize only occurs during delete, so no worry at insert stage
    if (((float)d->count+1) / (float)d->size > UPSIZE_THRESHOLD){
        upsize(d);
    }
    
    // insert a value into dictionary
    // do the hash first
    long key_hash = hash_index(key, P, M);
    key_hash %= d->size; // d->size causes the segmentation fault

    /*if the position is null: simply insert
      if the position is not null, check if the key is same
        if key is same, update the value
        if not same, find along the collision chain
            if find null, then insert
            if not null and find the same key, simply update
    
    so i separate into two parts: 
    first determine the location, then update/insert */

    if (*(d->bucket_array + key_hash) != NULL){
        if (strcmp((*(d->bucket_array + key_hash))->key, key) != 0){
            while (true){
                key_hash++;
                key_hash %= d->size;
                if(*(d->bucket_array + key_hash) == NULL 
                    || strcmp((*(d->bucket_array + key_hash))->key, key) == 0){
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
        update_item((*(d->bucket_array + key_hash))->value, value);
    }

    return;
}


// modify the delete function, even you delete, the key is still there
// and the value is set to null. So that the collision chain is not broken
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
            /*do not free the key, only free the value
              so that the collision chain is not broken
              so in the item structure, char* key still exist
              only char* value is freeed*/
            free((*(d->bucket_array+key_hash))->value);
            (*(d->bucket_array+key_hash))->value = NULL;
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


// need to modify the search function to suit the delete
// when you find the key, need to check if the value is null or not
// return null if not find, else just return the value
char* search(const Dictionary d, char*key){

    long key_hash = hash_index(key, P, M);
    key_hash %= d->size;

    if (*(d->bucket_array+key_hash) == NULL){
        return NULL;
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
                    return NULL;
                }
                else if (strcmp( (*(d->bucket_array+key_hash))->key, key) == 0){
                    // find the same key, check if the value is null
                    if ((*(d->bucket_array+key_hash))->value == NULL){
                        return NULL;
                    }
                    else{
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
}


// return some status information 
void status(const Dictionary d){

    printf("Size = %ld   Item number = %ld   ",
            d->size, d->count);
    
    if (d->count != 0){
        printf("Load factor = %.0f%%", (float)(d->count) / (float)d->size * 100.0);
    }
    printf("\n");
    return;
}


// this function prints out all the key-value pair
// need to skip those deleted items (check if value is null)
void view_pairs(const Dictionary d){

    printf("\nDictionary key-value pairs:\n");

    for (int i = 0; i < d->size; i++){
        if (*(d->bucket_array + i) != NULL 
                && (*(d->bucket_array+i))->value != NULL){
            printf("[%s] -> [%s]\n", 
                (*(d->bucket_array+i))->key, (*(d->bucket_array+i))->value);
        }
    }

    printf("\n");
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
    printf("Original status: ");
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

    printf("New status: ");
    status(d);
    printf("\n");
    return;
}


/* downsize function: called in the "delete" function
   the downsize function reduces to max(d->size / 2, START_SIZE)
   modify to suit the new "delete"
   during downsizing, collision chain removes those deleted node
   but make sure the chain is still linked */
void downsize(Dictionary d){

    printf("Dictionary downsizing...\n");
    printf("Original status: ");
    status(d);

    Dictionary dnew;

    // the size is chosen with max(current/2, START_SIZE)
    if ((d->size / 2) > START_SIZE){
        dnew = create((long) (d->size / 2));
    }
    else{
        dnew = create((long) START_SIZE);
    }

    dnew->count = d->count; // the number in dic is not changed

    //now need to copy all values to here
    // scan through d, find a key-value, then re-calculate the hash and allocate
    // if value is null, omit
    for (long i = 0; i < d->size; i++){
        if (*(d->bucket_array + i) != NULL 
                && (*(d->bucket_array+i))->value != NULL){
            
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

    printf("New status: ");
    status(d);
    printf("\n");
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
void update_item(char* item_p_value, char* value){
    // need to check if the item value is null or not
    if (item_p_value == NULL){
        item_p_value = (char*)malloc(strlen(value)*sizeof(char));
    }
    else{
        item_p_value = realloc(item_p_value, strlen(value)*sizeof(char));
    }

    if (item_p_value == NULL){
        printf("Memory allocation fail: update item\n");
        exit(EXIT_FAILURE);
    }

    strcpy(item_p_value, value);
    return;
}
