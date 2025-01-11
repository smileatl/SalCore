#include "kill_bunny.h"

void* KillerBunny::create(Base_ObjectParams*)
{
    return new KillerBunny();
}

base_int32_t KillerBunny::destroy(void* p)
{
    if (!p) {
        return -1;
    }
    delete (KillerBunny*)p;
    return 0;
}

KillerBunny::KillerBunny()
{
}

KillerBunny::~KillerBunny()
{
}
