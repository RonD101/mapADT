/* ----------------------------------------------------------------------------
                        Random Key Tester - map.c
This tester will generate random VALID keys and values, and run random tests
with them by using different map.c functions. Please compile with 'map.c', and
make sure the test file is located in the 'tests' folder. For best feedback
the test should be run by valgrind, and use the compilation flags noted in the
assignment pdf file.
And for the sake of our server, and for your own, DO NOT EXCEED 100 tests - it
will take forever and put a strain on poor csl3. Enjoy!
------------------------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
//#include "../mtm_map/map.h"
#include "map.h"

#define KEY_LEN 10
#define VAL_LEN 100
#define KEY_OPTIONS 10
#define VAL_OPTIONS 27
#define STRING_END '\0'
#define TEST_OPTIONS 1
#define NUM_OF_TESTS 3
#define MAX_ERROR_NAME_LEN 25

void randomizeKey(char* key);
void randomizeVal(char* val);
void runMapRandomTest(Map map, char* key, char* val, FILE* log_file);
MapResult runMapPutTest(Map map, char* key, char* val, FILE* log_file);
MapResult runMapCopyTest(Map map, char* key, FILE* log_file);
MapResult runMapRemoveTest(Map map, char* key, char* val, FILE* log_file);
void printMapErrorMessage(const char* func_name, const char* key, const char* val, MapResult error, FILE* log_file);
void printMemoryError(const char* func_name, FILE* log_file);

int main(){

    FILE* log_file = fopen("testlog.txt", "a");
    if(log_file == NULL){
        printf("Log file could not be open. Exiting...");
        return 0;
    }
    char *key = calloc((KEY_LEN + 1), sizeof(char));
    if(key == NULL){
        printMemoryError("main", log_file);
        return 0;
    }
    char *val = calloc((VAL_LEN + 1), sizeof(char));
    if(val == NULL){
        printMemoryError("main", log_file);
        free(key);
        return 0;
    }
    Map map = mapCreate();
    if(map == NULL){
        printMemoryError("main", log_file);
        return 0;
    }

    time_t t;
    time(&t);
    unsigned int seed = time(0);
    srand(seed);

    printf("Running all tests, please be patient.\n");
    fprintf(log_file, "\n\n*************************************************************\n");
    fprintf(log_file, "\tTest date and time: %s", ctime(&t));
    fprintf(log_file, "*************************************************************\n\n");
    fprintf(log_file, "Srand() seed: %d\n", seed); // for recreation purposes

    for(int i=0; i<NUM_OF_TESTS; i++){
        printf("Main loop: [%d of %d]\r", i+1, NUM_OF_TESTS);
        runMapRandomTest(map, key, val, log_file);
        mapClear(map); // Clear map after every NUM_OF_TESTS tests to avoid memory filling
    }

    free(key);
    free(val);
    mapDestroy(map);
    printf("\nTest complete. Please check \"testlog.txt\" for more information.\n");
    printf("If you can't see anything in the file, you made it (at least error-wise)!\nPress any key to exit...\n");
    fclose(log_file);
    getchar();
    return 0;
}

void randomizeKey(char* key){
    char key_possible_chars[KEY_OPTIONS] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int random_len = rand() % KEY_LEN + 1;

    for(int i=0; i<random_len; i++){
        int random_index = rand() % KEY_OPTIONS;
        key[i] = key_possible_chars[random_index];
    }
    key[random_len] = STRING_END;
}

void randomizeVal(char* val){
    char val_possible_chars[VAL_OPTIONS] = {' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', \
                                            'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', \
                                            't', 'u', 'v', 'w', 'x', 'y', 'z'};
    int random_len = rand() % VAL_LEN + 1;

    for(int i=0; i<random_len; i++){
        int random_index = rand() % VAL_OPTIONS;
        val[i] = val_possible_chars[random_index];
    }
    val[random_len] = STRING_END;
}

void runMapRandomTest(Map map, char* key, char* val, FILE* log_file){
    int test;
    MapResult result;
    randomizeKey(key);
    randomizeVal(val);
    for(int i=0; i<NUM_OF_TESTS; i++){
        test = rand() % TEST_OPTIONS;
        switch(test){
            case 0:
                result = runMapPutTest(map, key, val, log_file); //put random keys and values inside, make sure they are identical to the input. Also tests mapContains and mapGet.
                break;
            case 1:
                result = runMapCopyTest(map, key, log_file); //create maps, copy into them, compare and destroy
                break;
            case 2:
                result = runMapRemoveTest(map, key, val, log_file); //put keys in a map, and make sure mapRemove is removing all of them.
                break;
        }
        if(result != MAP_SUCCESS){
            break;
        }
    }
}

MapResult runMapPutTest(Map map, char* key, char* val, FILE* log_file){
    assert(map && key && val);
    MapResult result = MAP_SUCCESS;
    for(int i=0; i<NUM_OF_TESTS; i++){
        int case_value = rand() % 2;
        switch(case_value){
            case 0:
                randomizeKey(key);
                randomizeVal(val);
                break;
            case 1:
                randomizeVal(val);
                break;
        }

        result = mapPut(map, key, val);
        if(result != MAP_SUCCESS){
            printMapErrorMessage("mapPut", key, val, result, log_file);
            break;
        }
        if(!mapContains(map, key)){
            printMapErrorMessage("mapContains", key, val, MAP_ITEM_DOES_NOT_EXIST, log_file);
            break;
        }
        if(strcmp(mapGet(map, key), val) != 0){
            printMapErrorMessage("mapGet", key, val, MAP_ERROR, log_file);
            break;
        }
    }
    return result;
}

MapResult runMapCopyTest(Map map, char* key, FILE* log_file){
    MapResult result = MAP_SUCCESS;
    for(int i=0; i<NUM_OF_TESTS; i++){
        Map new_map = mapCopy(map);
        if(new_map == NULL){
            printMemoryError("runMapCopyTest", log_file);
            continue;
        }
        char* key_iterator = mapGetFirst(map);
        while(key_iterator != NULL){
            if(strcmp(mapGet(map, key_iterator), mapGet(new_map, key_iterator)) != 0){
                result = MAP_ERROR;
                printMapErrorMessage("mapCopy", key_iterator, mapGet(map, key), result, log_file);
                break;
            }
            key_iterator = mapGetNext(map);
        }
        mapDestroy(new_map);
    }
    return result;
}

MapResult runMapRemoveTest(Map map, char* key, char* val, FILE* log_file){
    MapResult result_map;
    MapResult result_new_map;

    for(int j=0; j<NUM_OF_TESTS; j++){
        result_map = mapClear(map);
        if(result_map != MAP_SUCCESS){
            printMapErrorMessage("mapClear", "", "", result_map, log_file);
            return result_map;
        }
        Map new_map = mapCreate();
        if(new_map == NULL){
            printMemoryError("runMapRemoveTest", log_file);
            return MAP_ERROR;
        }

        for(int i=0; i<NUM_OF_TESTS; i++){
            randomizeKey(key);
            randomizeVal(val);
            result_map = mapPut(map, key, val);
            if(result_map != MAP_SUCCESS){
                printMapErrorMessage("mapPut", key, val, result_map, log_file);
                break;
            }
            result_new_map = mapPut(new_map, key, val);
            if(result_new_map != MAP_SUCCESS){
                printMapErrorMessage("mapPut", key, val, result_new_map, log_file);
                break;
            }
        }

        if(result_new_map == MAP_SUCCESS && result_map == MAP_SUCCESS){
            int new_map_size = mapGetSize(new_map);
            int counter = 0;
            char *key_iterator = mapGetFirst(map);
            while(key_iterator != NULL){
                result_new_map = mapRemove(new_map, key_iterator);
                if(result_new_map != MAP_SUCCESS){
                    printMapErrorMessage("mapRemove", "", "", result_new_map, log_file);
                    break;
                }

                counter++;
                if(mapGetSize(new_map) != (new_map_size - counter)){
                    printMapErrorMessage("mapRemove", "", "", MAP_ERROR, log_file);
                    break;
                }
                key_iterator = mapGetNext(map);
            }
        }
        mapDestroy(new_map);
    }
    return result_new_map;
}

void printMapErrorMessage(const char* func_name, const char* key, const char* val, MapResult error, FILE* log_file){
    assert(error != MAP_SUCCESS);
    char str[MAX_ERROR_NAME_LEN] = {0};
    switch(error){
        case MAP_SUCCESS:
            strcpy(str, "MAP_SUCCESS");
            break;
        case MAP_OUT_OF_MEMORY:
            strcpy(str, "MAP_OUT_OF_MEMORY");
            break;
        case MAP_NULL_ARGUMENT:
            strcpy(str, "MAP_NULL_ARGUMENT");
            break;
        case MAP_ITEM_ALREADY_EXISTS:
            strcpy(str, "MAP_ITEM_ALREADY_EXISTS");
            break;
        case MAP_ITEM_DOES_NOT_EXIST:
            strcpy(str, "MAP_ITEM_DOES_NOT_EXIST");
            break;
        case MAP_ERROR:
            strcpy(str, "MAP_ERROR");
            break;
    }
    fprintf(log_file, "\n- Function: \"%s\" -> received error: \"%s\"\n", func_name, str);
    fprintf(log_file, "\tKey used: \"%s\"\n", key);
    fprintf(log_file, "\tValue used: \"%s\"\n", val);
}

void printMemoryError(const char* func_name, FILE* log_file){
    fprintf(log_file, "\n- Memory allocation error occured in function: \"%s\"\n", func_name);
}
