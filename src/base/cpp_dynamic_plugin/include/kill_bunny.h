#ifndef KILLER_BUNNY_H
#define KILLER_BUNNY_H

#include "base.h"

class KillerBunny
{
public:
  // static plugin interface
    static void * create(Base_ObjectParams *); 
    static base_int32_t destroy(void *);
    ~KillerBunny();

private:
    KillerBunny();
};

#endif // KILLER_BUNNY_H