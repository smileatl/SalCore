#include "fidgety_phantom.h"

void * FidgetyPhantom::create(Base_ObjectParams *)
{
  return new FidgetyPhantom();
}

base_int32_t FidgetyPhantom::destroy(void * p)
{
  if (!p)
    return -1;
  delete (FidgetyPhantom *)p;
  return 0;
}

FidgetyPhantom::FidgetyPhantom()
{
}

FidgetyPhantom::~FidgetyPhantom()
{
}