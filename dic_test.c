#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dic_function.h"

int main(void){
    Dictionary d;
    d = create(START_SIZE);
    status(d);

    FILE* fp;
    fp = fopen("dic_test.txt", "r");
    if (fp == NULL){
        printf("Error open file\n");
        exit(EXIT_FAILURE);
    }

    char key[30];
    char value[30];

    /////////////////////////////////////////////////////////////
    printf("Command: Insert 125 groups key-value pairs\n\n");
    while ((fscanf(fp, "%s", key)==1) && (fscanf(fp, "%s", value)==1)){
        insert(d, key, value);
    }

    printf("finish inserting\n");
    status(d);
    fclose(fp);
    fp = NULL;
    view_pairs(d);
    /////////////////////////////////////////////////////////////
    
    printf("Command: Downsize, delete 100 keys\n\n");
    fp = fopen("dic_test.txt", "r");
    if (fp == NULL){
        printf("Error open file\n");
        exit(EXIT_FAILURE);
    }
    int counter = 0;
    while ((fscanf(fp, "%s", key)==1) && (fscanf(fp, "%s", value)==1)){
        counter++;
        if (counter > 100){
            break;
        }
        delete(d, key);
    }

    printf("finish deleting.\n");
    status(d);
    fclose(fp);
    //////////////////////////////////////////////////////////////

    printf("Command: Check some deleted key and not-existing key\n\n");
    printf("key = [a], [aaa], [able], [abuse], [xx], [night], [bad], [work]\n");
    char *test_key[] = {"a", "aaa", "able", "abuse","xx", "night", "bad", "work"};
    int test_key_length = 8;
    for (int i = 0; i < test_key_length; i++){
        char* result = search(d, test_key[i]);
        if (result == NULL){
            printf("Error: no key match [%s]\n", test_key[i]);
        }
        else{
            printf("[%s]->[%s]\n", test_key[i], result);
        }
    }
    //////////////////////////////////////////////////////////////

    printf("\nCommand: Check some existing key-value pairs\n");
    printf("[agencies]->[agency], [aging]->[ago], [aid]->[aids], [aims]->[air]\n");
    printf("check return %s\n", search(d, "agencies"));
    printf("check return %s\n", search(d, "aging"));
    printf("check return %s\n", search(d, "aid"));
    printf("check return %s\n", search(d, "aims"));
    //////////////////////////////////////////////////////////////

    printf("\nCommand: Consider [aging]->[ago], repeatedly insert different values\n");
    printf("Command: insert[aging]->[hello], [gello], [yogurt], [tuna], [happy]\n");
    printf("Then check the value of key [aging]\n");
    insert(d, "aging", "hello");
    insert(d, "aging", "gello");
    insert(d, "aging", "yogurt");
    insert(d, "aging", "tuna");
    insert(d, "aging", "happy");
    printf("check return %s\n", search(d, "aging"));
    //////////////////////////////////////////////////////////////

    printf(("\nFinish test, deleting...\nThank you.\n"));
    destroy(d);
    d = NULL;
    return 0;
}