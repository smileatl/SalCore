#include <stdlib.h>
#include "mellow_monster.h"

typedef struct MellowMonster
{
    /* additional monster-specific data */
    base_uint32_t dummy;

} MellowMonster;

void* MellowMonster_create(Base_ObjectParams* params)
{
    printf("Hello MellowMonster_create\n");
    MellowMonster* mm = (MellowMonster*)malloc(sizeof(MellowMonster));

    return mm;
}


base_int32_t MellowMonster_destroy(void* p)
{
    printf("Hello MellowMonster_destroy\n");
    if (!p)
        return -1;
    free((MellowMonster*)p);
    return 0;
}