//
// Created by Ron Dahan on 14/04/2020.
//

#include "map.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct Map_t
{
    char** key;
    char** value;
    int size;
    int maxSize;
    int iterator;  // next index to return
};

Map mapCreate(){
    Map map = malloc(sizeof(*map));
    return NULL;
}

enum MapResult_t mapRemove(Map map, const char* key)
{
    int cmpResult = 1, i = 0;
    if(map == NULL || key == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    while(cmpResult != 0 && i < map->size)
    {
        cmpResult = strcmp(key, map->key[i]);
        i++;
    }
    if(cmpResult == 0)
    {
        free(map->key[i]);
        free(map->value[i]);
        map->size--;
        return MAP_SUCCESS;
    }
    return MAP_ITEM_DOES_NOT_EXIST;
}

MapResult mapClear(Map map)
{
    if(map)
    {
        while (mapGetSize(map) > 0)
        {
            mapRemove(map, mapGetFirst(map));
        }
        return MAP_SUCCESS;
    }
    return MAP_NULL_ARGUMENT;
}

void mapDestroy(Map map)
{
    if(map)
    {
        mapClear(map);
        free(map->key);
        free(map->value);
        free(map);
    }
}

char* mapGetNext(Map map)
{
    assert(map != NULL);
    if(map->iterator >= map->size)
    {
        return NULL;
    }
    return map->key[map->iterator++];
}

char* mapGetFirst(Map map)
{
    assert(map != NULL);
    map->iterator = 0;
    return mapGetNext(map);
}

