//
// Created by Ron Dahan on 14/04/2020.
//
#include <stdlib.h>
#include "map.h"

#define INITIAL_SIZE 2
#define EXPAND_FACTOR 2
static MapResult expand(Map map);

struct Map_t{
    char** key;
    char** value;
    int size;
    int maxSize;
    int iterator;
};

Map mapCreate(){
    Map new_map = malloc(sizeof(*new_map));
    if (new_map == NULL)
        return NULL;
    new_map->key = malloc(INITIAL_SIZE * sizeof(char*));
    new_map->value = malloc(INITIAL_SIZE * sizeof(char*));
    if(new_map->key == NULL || new_map->value == NULL){ //checking id any of the malloc failed
        free(new_map->key);
        free(new_map->value);
        free(new_map);
        return NULL;
    }
    new_map->size = 0;
    new_map->maxSize = INITIAL_SIZE;
    return new_map;
}

Map mapCopy(Map map){
    if(map == NULL){
        return NULL;
    }
    Map newMap = mapCreate();
    if(newMap == NULL)
        return NULL;
    newMap->size = map->size;
    newMap->maxSize = map->maxSize;
    for (int i = 0; i < map->size; ++i) {//copying the original map data to the new map
        newMap->key[i] = malloc(strlen(map->key[i])+1);
        newMap->value[i] = malloc(strlen(map->value[i])+1);
        if(newMap->key[i] == NULL || newMap->value[i] == NULL){
            mapDestroy(newMap);
            return NULL;
        }
        strcpy(newMap->key[i],map->key[i]);
        strcpy(newMap->key[i],map->key[i]);
    }
    return newMap;
}

int mapGetSize(Map map){
    if(map == NULL)
        return -1;
    return map->size;
}

MapResult mapRemove(Map map, const char* key)
{
    int cmpResult = 1, i = 0;
    //checks if the arguments passed were NULL
    if(map == NULL || key == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    //searches map to see if key element exists
    while(cmpResult != 0 && i < map->size)
    {
        cmpResult = strcmp(key, map->key[i]);
        i++;
    }
    //if key element exists, free its data and the data associated with it
    if(cmpResult == 0)
    {
        free(map->key[i]);
        free(map->value[i]);
        //puts elements from end of map to where index was for removal
        map->key[i] = map->key[map->size];
        map->value[i] = map->value[map->size];
        map->size--; //reduce map size by one once element is removed
        return MAP_SUCCESS;
    }
    return MAP_ITEM_DOES_NOT_EXIST; //returns if key element was not found in map
}

MapResult mapClear(Map map)
{
    if(map)
    {
        //removes each element from map via function mapRemove until map.size is 0
        while (mapGetSize(map) > 0)
        {
            mapRemove(map, mapGetFirst(map));
        }
        return MAP_SUCCESS;
    }
    return MAP_NULL_ARGUMENT; //returns if map is NULL
}

void mapDestroy(Map map)
{
    if(map)
    {
        // uses function mapClear then deallocates map elements and its data
        mapClear(map);
        free(map->key);
        free(map->value);
        free(map);
    }
}

char* mapGetNext(Map map)
{
    //if reached end of map or if map argument is NULL return NULL
    if(map->iterator >= map->size || map == NULL)
    {
        return NULL;
    }
    return map->key[map->iterator++]; //return next key
}

char* mapGetFirst(Map map)
{
    //if map argument is NULL return NULL
    if(map == NULL){
        return NULL;
    }
    //Sets the internal iterator to the first key element in the map
    map->iterator = 0;
    return mapGetNext(map); //returns first element in map
}


char* mapGet(Map map, const char* key){
    if(map == NULL || key == NULL){
        return NULL;
    }
    for (int i = 0; i < map->size; ++i) { //searching for the key in map
        if(strcmp(map->key[i],key) == 0){
            return map->value[i];
        }
    }
    return NULL;
}

bool mapContains(Map map, const char* key){
    if(map == NULL || key == NULL){
        return false;
    }
    if(mapGet(map,key) == NULL){ //using the mapGet function to check if the key exist in map
        return false;
    }
    return true;
}

MapResult mapPut(Map map, const char* key, const char* data){
    if(map == NULL || key == NULL || data == NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(mapContains(map,key)){ //check if key exist then update value
        char* value = (mapGet(map,key));
        value = realloc(value,strlen(data)+1);
        if(value == NULL){
            return MAP_OUT_OF_MEMORY;
        }
        strcpy(value,data);
        return MAP_SUCCESS;
    } else{ //if key isn't exist. than we create him and associate data as is value
        if(map->size == map->maxSize){ //expanding map if it reach is max size
            if(expand(map) == MAP_OUT_OF_MEMORY){
                return MAP_OUT_OF_MEMORY;
            }
        }
        map->key[map->size] = malloc(sizeof(strlen(key)+1));
        map->value[map->size] = malloc(sizeof(strlen(data)+1));
        if(map->value[map->size] == NULL || map->key[map->size] == NULL){
            free(map->value[map->size]);
            free(map->key[map->size]);
            return MAP_OUT_OF_MEMORY;
        }
        strcpy(map->key[map->size],key);
        strcpy(map->value[map->size],data);
        map->size += 1;
        return MAP_SUCCESS;
    }
    // Shouldn't reach to this point
    return MAP_ERROR;
}

//expand function trying to reallocate make space to keys and values array
static MapResult expand(Map map) {
    int newSize = EXPAND_FACTOR * map->maxSize;
    char** newKey = realloc(map->key, newSize * sizeof(char*));
    char** newValue = realloc(map->value, newSize * sizeof(char*));
    if (newKey == NULL || newValue == NULL) {
        free(newKey);
        free(newValue);
        return MAP_OUT_OF_MEMORY;
    }
    //updating the pointers
    map->key = newKey;
    map->value = newValue;
    map->maxSize = newSize;
    return MAP_SUCCESS;
}