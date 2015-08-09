#ifndef _AUTORELEASEPOOL
#define _AUTORELEASEPOOL

#include "ReferenceCounted.h"
#include <list>
#include <mutex>

class AutoreleasePool : public ReferenceCounted
{
  private:
    AutoreleasePool *parent;
    std::list<ReferenceCounted *> pendingReleases;

    std::mutex insertLock;

  public:
    static AutoreleasePool *currentPool();

    AutoreleasePool();
    AutoreleasePool(const AutoreleasePool& rhs) = delete;
    AutoreleasePool& operator=(const AutoreleasePool& rhs) = delete;

    void addObject(ReferenceCounted *toAutorelease);
    void drain();

};

#endif
