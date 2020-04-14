//
// Created by Ron Dahan on 14/04/2020.
//
#include <stdlib.h>
#include "map.h"

struct Map_t{
    char** key;
    char** value;
};

void mapDestroy(Map map) {

}

Map mapCreate(){
    Map map = malloc(sizeof(*map));
    return NULL;
}
