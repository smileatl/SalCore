#ifndef FIDGETY_PHANTOM
#define FIDGETY_PHANTOM

#include <base.h>

class FidgetyPhantom
{
public:
    // static plugin interface
    static void* create(Base_ObjectParams*);
    static base_int32_t destroy(void*);
    ~FidgetyPhantom();

private:
    FidgetyPhantom();
};

#endif // FIDGETY_PHANTOM