#include "Autorelease.h"

#if /*shouldLog */ 1

#include <iostream>
#define debugLog(string) std::cerr << "===== In autoreleasepool " << this \
  << " " << string << " =====" << std::endl;
#else
#define debugLog(string)
#endif

static thread_local AutoreleasePool *_currentPool = nullptr;

AutoreleasePool *AutoreleasePool::currentPool()
{
  return _currentPool;
}


AutoreleasePool::AutoreleasePool()
{
  debugLog("In constructor to stack on top of " << _currentPool);
  parent = _currentPool;
  _currentPool = this;
}

void AutoreleasePool::addObject(ReferenceCounted *toAutorelease)
{
  if(toAutorelease->referenceCount() == -1)
  {
    throw std::runtime_error("Only objects on heap can be autoreleased");
  }
  debugLog("Adding object " << toAutorelease << " to pool")
  insertLock.lock();
  pendingReleases.push_back(toAutorelease);
  insertLock.unlock();
}

void AutoreleasePool::drain()
{
  debugLog("Draining, will pop autoreleasepool " << parent);
  insertLock.lock();
  auto rIterator = pendingReleases.rbegin();
  while(rIterator != pendingReleases.rend())
  {
    debugLog("Releasing object " << *rIterator);
    ReferenceCounted *toRelease = *rIterator;
    toRelease->release();
    rIterator++;
  }
  insertLock.unlock();

  _currentPool = parent;

  // If this autoreleasepool is on the heap, draining should delete it.
  if(referenceCount() != -1)
  {
    delete this;
  }
}

