#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct cache_node{
    unsigned long tag;
    bool valid;
    long index;
};


//function prototypes
struct cache_node*** constructCache(unsigned long sizeOfCache, unsigned long associavity, unsigned long blockSize);
void performTask(char* file, struct cache_node*** cacheCf, struct cache_node*** cacheCs, unsigned long cacheSizeCf, unsigned long cacheSizeCs, unsigned long associavityCf, unsigned long associavityCs, unsigned long blockSize, char* policyCf, char* policyCs);
unsigned long intPow(int base, int power);
int myLog(unsigned long blockSize);
void freeCache(struct cache_node*** cache, unsigned long associavity, unsigned long numberOfSet);


int main(int argc, char** argv){
    //checking number of arguments 
    if(argc != 9){
        printf("Please enter correct number of arguments\n");
        exit(EXIT_SUCCESS);
    }
    unsigned long cacheSizeCf;
    sscanf(argv[1], "%lu", &cacheSizeCf);

    int lengthCf = strlen(argv[2]);
    char temp[20];
    strcpy(temp, argv[2]);
    unsigned long associavityCf = 0;
    for(int i = lengthCf - 1; i >= 6; i--){
        unsigned long factor = intPow(10, lengthCf - i -1);
        int digit = temp[i] - '0';
        associavityCf += digit * factor;
    }

    char* policyCf = argv[3];
    unsigned long blockSize;
    sscanf(argv[4], "%lu", &blockSize);

    unsigned long cacheSizeCs;
    sscanf(argv[5], "%lu", &cacheSizeCs);
    int lengthCs = strlen(argv[6]);
    strcpy(temp, argv[6]);
    unsigned long associavityCs = 0;
    for(int i = lengthCs - 1; i >= 6; i--){
        unsigned long factor = intPow(10, lengthCs - i -1);
        int digit = temp[i] - '0';
        associavityCs += digit * factor;
    }
    char* policyCs = argv[7];
    char* file = argv[8];

    struct cache_node*** cacheCf = constructCache(cacheSizeCf, associavityCf, blockSize);
    struct cache_node*** cacheCs = constructCache(cacheSizeCs, associavityCs, blockSize);
    performTask(file, cacheCf, cacheCs, cacheSizeCf, cacheSizeCs, associavityCf, associavityCs, blockSize, policyCf, policyCs);

    unsigned long numberOfSetCf = cacheSizeCf / (associavityCf * blockSize);
    unsigned long numberOfSetCs = cacheSizeCs / (associavityCs * blockSize);
    freeCache(cacheCf, associavityCf, numberOfSetCf);
    freeCache(cacheCs, associavityCs, numberOfSetCs);
    return EXIT_SUCCESS;

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


unsigned long intPow(int base, int power){
    int result = 1;
    for(int i = 0; i < power; i++){
        result = result * base;
    }
    return result;
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
    //free the cache
    for(unsigned long i = 0; i < numberOfSet; i++){
        for(unsigned long j = 0; j < associavity; j++){
            free(cache[i][j]);
        }
        free(cache[i]);
    }
    free(cache);
    return;
}


void performTask(char* file, struct cache_node*** cacheCf, struct cache_node*** cacheCs, unsigned long cacheSizeCf, unsigned long cacheSizeCs, unsigned long associavityCf, unsigned long associavityCs, unsigned long blockSize, char* policyCf, char* policyCs){
    FILE* information = fopen(file, "r");
    if(information == NULL){
        printf("Couldn't open the file\n");
        exit(EXIT_SUCCESS);
    }
    unsigned long memRead = 0;
    unsigned long memWrite = 0;
    unsigned long cacheHitCf = 0;
    unsigned long cacheMissCf = 0;
    unsigned long cacheHitCs = 0;
    unsigned long cacheMissCs = 0;
    unsigned long indexCf = 0;
    unsigned long indexCs = 0;
    
    while(!feof(information)){
        //setting up the variables 
        unsigned long number;
        char type;
        fscanf(information, "%c %lx\n", &type, &number);
        int bShiftAmount = myLog(blockSize);

        int sShiftAmountCf = myLog(cacheSizeCf / (associavityCf * blockSize));
        int sShiftAmountCs = myLog(cacheSizeCs / (associavityCs * blockSize));
        
        unsigned long tagCf = number >> (bShiftAmount + sShiftAmountCf);
        unsigned long tagCs = number >> (bShiftAmount + sShiftAmountCs);

        int setIndexCf = (number - (tagCf << (bShiftAmount + sShiftAmountCf))) >> bShiftAmount;
        int setIndexCs = (number - (tagCs << (bShiftAmount + sShiftAmountCs))) >> bShiftAmount;

        //found in first cache

        bool found = false;
        for(unsigned long i = 0; i < associavityCf; i++){
            if(cacheCf[setIndexCf][i] -> valid == true){
                if(cacheCf[setIndexCf][i] -> tag == tagCf){
                    cacheHitCf++;
                    if(strcmp(policyCf, "lru") == 0){
                        cacheCf[setIndexCf][i] -> index = indexCf;
                    }
                    if(type == 'W'){
                        memWrite++;
                    }
                    found = true;
                    break;
                }
            }
        }
        if(found == true){
            indexCf++;
            indexCs++;
            continue;
        }
        //not found in first chache and if empty
        else if(found == false){
            cacheMissCf++;
            //checking Cache 2
            for(unsigned long i = 0; i < associavityCs; i++){
                if(cacheCs[setIndexCs][i] -> valid == true){
                    if((cacheCs[setIndexCs][i] -> tag) == tagCs){
                        cacheHitCs++;
                        found = true;
                        cacheCs[setIndexCs][i] -> valid = false;
                    
                        if(type == 'W'){
                            memWrite++;
                        }
                        break;
                    }
                }
            }
            //memory accessed 
            if(found == false){
                cacheMissCs++;
                if(type == 'R'){
                    memRead++;
                }
                else{
                    memRead++;
                    memWrite++;
                }
            }
        }

        bool placer = false;
        //L1 cache is empty
        for(unsigned long i = 0; i < associavityCf; i++){
            if(cacheCf[setIndexCf][i] -> valid == false){
                cacheCf[setIndexCf][i] -> valid = true;
                cacheCf[setIndexCf][i] -> tag = tagCf;
                cacheCf[setIndexCf][i] -> index = indexCf;
                
                placer = true;
                break;
            }
        }

        //first cache not empty
        if(placer == false){
            unsigned long first = cacheCf[setIndexCf][0] -> index;
            unsigned long indexer = 0;
            for(unsigned long i = 1; i < associavityCf; i++){
                if(first > cacheCf[setIndexCf][i] -> index){
                    first = cacheCf[setIndexCf][i] -> index;
                    indexer = i;
                } 
            }
            //updating tag and setIndex for L2 cache
            number = ((cacheCf[setIndexCf][indexer] -> tag) << (sShiftAmountCf + bShiftAmount)) + (setIndexCf << bShiftAmount);
            tagCs = number >> (bShiftAmount + sShiftAmountCs);
            setIndexCs = (number - (tagCs << (bShiftAmount + sShiftAmountCs))) >> bShiftAmount;
            
            cacheCf[setIndexCf][indexer] -> tag = tagCf;
            cacheCf[setIndexCf][indexer] -> index = indexCf;

            //cache 2 is empty
            for(unsigned long i = 0; i < associavityCs; i++){
                if(cacheCs[setIndexCs][i] -> valid == false){
                    cacheCs[setIndexCs][i] -> valid = true;
                    cacheCs[setIndexCs][i] -> tag = tagCs;
                    cacheCs[setIndexCs][i] -> index = indexCs;
                    placer = true;
                    break;
                }
            }
            //cache 2 is not empty
            if(placer == false){
                first = cacheCs[setIndexCs][0] -> index;
                indexer = 0;
                for(unsigned long i = 1; i < associavityCs; i++){
                    if(first > cacheCs[setIndexCs][i] -> index){
                        first = cacheCs[setIndexCs][i] -> index;
                        indexer = i;
                    } 
                }
                cacheCs[setIndexCs][indexer] -> tag = tagCs;
                cacheCs[setIndexCs][indexer] -> index = indexCs;
                cacheCs[setIndexCs][indexer] -> valid = true;
            }
        }
        indexCf++;
        indexCs++;

    }
    //printing result
    printf("memread:%lu\n", memRead);
    printf("memwrite:%lu\n", memWrite);
    printf("l1cachehit:%lu\n", cacheHitCf);
    printf("l1cachemiss:%lu\n", cacheMissCf);
    printf("l2cachehit:%lu\n", cacheHitCs);
    printf("l2cachemiss:%lu\n", cacheMissCs);
    fclose(information);
    return;
}
