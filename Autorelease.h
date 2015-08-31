#ifndef _AUTORELEASEPOOL
#define _AUTORELEASEPOOL

#include "ReferenceCounted.h"
#include <mutex>

#define NodeBufferSize 8
struct AutoreleaseStructure
{
  AutoreleaseStructure *parent;
  ReferenceCounted *buffer[NodeBufferSize];
};

class AutoreleasePool : public ReferenceCounted
{
  private:
    std::mutex insertLock;
    AutoreleasePool *parent;

    AutoreleaseStructure *pendingReleases;
    int currentBufferIndex;

  public:
    static AutoreleasePool *currentPool();

    AutoreleasePool();
    AutoreleasePool(const AutoreleasePool& rhs) = delete;
    AutoreleasePool& operator=(const AutoreleasePool& rhs) = delete;

    void addObject(ReferenceCounted *toAutorelease);
    void drain();

};

#endif
