#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct cache_node{
    unsigned long tag;
    bool valid;
    long index;
};


//Function prototypes
unsigned long intPow(int base, int power);
struct cache_node*** constructCache(unsigned long sizeOfCache, unsigned long associavity, unsigned long blockSize);
int myLog(unsigned long blockSize);
void performTask(char* file, struct cache_node*** cache, unsigned long sizeOfCache, unsigned long associavity, unsigned long blockSize, char* policy);
void freeCache(struct cache_node*** cache, unsigned long associavity, unsigned long numberOfSet);


int main(int argc, char** argv){
    //checking number of arguments 
    if(argc != 6){
        printf("Please enter correct number of arguments\n");
        exit(EXIT_SUCCESS);
    }
    unsigned long sizeOfCache;
    sscanf(argv[1], "%lu", &sizeOfCache);

    int length = strlen(argv[2]);
    char temp[20];
    strcpy(temp, argv[2]);
    unsigned long associavity = 0;
    for(int i = length - 1; i >= 6; i--){
        unsigned long factor = intPow(10, length - i -1);
        int digit = temp[i] - '0';
        associavity += digit * factor;
    }


    char* policy = argv[3];
    unsigned long blockSize;
    sscanf(argv[4], "%lu", &blockSize);
    char* file = argv[5];
    
    struct cache_node*** cache = constructCache(sizeOfCache, associavity, blockSize);
    performTask(file, cache, sizeOfCache, associavity, blockSize, policy);
    
    unsigned long numberOfSet = sizeOfCache / (associavity * blockSize);
    freeCache(cache, associavity, numberOfSet);
    return EXIT_SUCCESS;
}


unsigned long intPow(int base, int power){
    int result = 1;
    for(int i = 0; i < power; i++){
        result = result * base;
    }
    return result;
}


struct cache_node*** constructCache(unsigned long sizeOfCache, unsigned long associavity, unsigned long blockSize){
    unsigned long numberOfSet = sizeOfCache / (associavity * blockSize);
    struct cache_node*** cache = malloc(sizeof(struct cache_node**) * numberOfSet);
    if(cache == NULL){
        printf("Unable to allocate memory\n");
        exit(EXIT_SUCCESS);
    }
    for(unsigned long i = 0; i < numberOfSet; i++){
        cache[i] = malloc(sizeof(struct cache_node*) * associavity);
        if(cache[i] == NULL){
            printf("Unable to allocate memory\n");
            exit(EXIT_SUCCESS);
        }
    }
    for(unsigned long j = 0; j < numberOfSet; j++){
        for(unsigned long k = 0; k < associavity; k++){
            cache[j][k] = malloc(sizeof(struct cache_node));
            if(cache[j][k] == NULL){
                printf("Unable to allocate memory\n");
                exit(EXIT_SUCCESS);
            }
            cache[j][k] -> valid = false;
        }
    }
    return cache;
}


void performTask(char* file, struct cache_node*** cache, unsigned long sizeOfCache, unsigned long associavity, unsigned long blockSize, char* policy){
    FILE* information = fopen(file, "r");
    if(information == NULL){
        printf("Couldn't open the file\n");
        exit(EXIT_SUCCESS);
    }
    unsigned long memRead = 0;
    unsigned long memWrite = 0;
    unsigned long cacheHit = 0;
    unsigned long cacheMiss = 0;
    unsigned long index = 0;
    while(!feof(information)){
        //setting up the variables 
        unsigned long number;
        char type;
        fscanf(information, "%c %lx\n", &type, &number);
        int bShiftAmount = myLog(blockSize);

        int sShiftAmount = myLog(sizeOfCache / (associavity * blockSize));
        
        unsigned long tag = number >> (bShiftAmount + sShiftAmount);
        int setIndex = (number - (tag << (bShiftAmount + sShiftAmount))) >> bShiftAmount;

        bool found = false;
        //cheching the cache
        for(unsigned long i = 0; i < associavity; i++){
            if(cache[setIndex][i] -> valid == true){
                if(cache[setIndex][i] -> tag == tag){
                    cacheHit++;
                    if(strcmp(policy, "lru") == 0){
                        cache[setIndex][i] -> index = index;
                    }
                    if(type == 'W'){
                        memWrite++;
                    }
                    found = true;
                    break;
                }
            }
        }
        if(found == false){
            cacheMiss++;
            bool placer = false;
            //placing in cache from the memery and its empty
            for(unsigned long i = 0; i < associavity; i++){
                if(cache[setIndex][i] -> valid == false){
                    cache[setIndex][i] -> valid = true;
                    cache[setIndex][i] -> tag = tag;
                    cache[setIndex][i] -> index = index;
                    placer = true;
                    break;
                }
            }
            //if cache is not empty 
            if(placer == false){
                unsigned long first = cache[setIndex][0] -> index;
                unsigned long indexer = 0;
                for(unsigned long i = 1; i < associavity; i++){
                    if(first > cache[setIndex][i] -> index){
                        first = cache[setIndex][i] -> index;
                        indexer = i;
                    } 
                }
                cache[setIndex][indexer] -> tag = tag;
                cache[setIndex][indexer] -> index = index;
            }
            if(type == 'R'){
                memRead++;
            }
            else{
                memRead++;
                memWrite++;
            }
        }
        index++;
    }
    //printing the results
    printf("memread:%lu\n", memRead);
    printf("memwrite:%lu\n", memWrite);
    printf("cachehit:%lu\n", cacheHit);
    printf("cachemiss:%lu\n", cacheMiss);
    fclose(information);
    return;
}


int myLog(unsigned long blockSize){
    int result = 0;
    while(blockSize != 1){
        blockSize = blockSize / 2;
        result++;
    }
    return result;
}


void freeCache(struct cache_node*** cache, unsigned long associavity, unsigned long numberOfSet){
    for(unsigned long i = 0; i < numberOfSet; i++){
        for(unsigned long j = 0; j < associavity; j++){
            free(cache[i][j]);
        }
        free(cache[i]);
    }
    free(cache);
    return;
}
