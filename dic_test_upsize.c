#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dic_function.h"

int main(void){
    Dictionary d;
    d = create(START_SIZE);
    status(d);
    printf("\n");

    FILE* fp;
    fp = fopen("dic_test_upsize.txt", "r");
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
    fp = fopen("dic_test_upsize.txt", "r");
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
    printf("key = [a], [aaa], [able], [abuse], [xx], [night], [bad], [work]");
    printf("%s", search(d, "a"));
    printf("%s", search(d, "aaa"));
    printf("%s", search(d, "able"));
    printf("%s", search(d, "abuse"));
    printf("%s", search(d, "xx"));
    printf("%s", search(d, "night"));
    printf("%s", search(d, "bad"));
    printf("%s", search(d, "work"));

    //////////////////////////////////////////////////////////////

    printf("\nCommand: Check some existing key-value pairs\n\n");
    printf("[agencies]->[agency], [aging]->[ago], [aid]->[aids], [aims]->[air]\n");
    printf("check return %s\n", search(d, "agencies"));
    printf("check return %s\n", search(d, "aging"));
    printf("check return %s\n", search(d, "aid"));
    printf("check return %s\n", search(d, "aims"));


    printf(("\nFinish test, deleting...\nThank you.\n"));
    destroy(d);
    d = NULL;
    return 0;
}