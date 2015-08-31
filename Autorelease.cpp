#include "Autorelease.h"

#if /*shouldLog */ 0

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

  pendingReleases = nullptr;
  currentBufferIndex = 0;

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

  if(currentBufferIndex == NodeBufferSize)
  {
    currentBufferIndex = 0;
  }

  if(currentBufferIndex == 0)
  {
    AutoreleaseStructure *newNode = new AutoreleaseStructure;
    newNode->parent = pendingReleases;
    pendingReleases = newNode;
  }
  pendingReleases->buffer[currentBufferIndex] = toAutorelease;
  currentBufferIndex++;

  insertLock.unlock();
}

void AutoreleasePool::drain()
{
  debugLog("Draining, will pop autoreleasepool " << parent);
  insertLock.lock();

  // The first buffer could be partial
  if(pendingReleases)
  {
    for(int i = 0; i < currentBufferIndex; i++)
    {
      pendingReleases->buffer[i]->release();
    }
    AutoreleaseStructure *toDelete = pendingReleases;
    pendingReleases = pendingReleases->parent;
    delete toDelete;
  }

  while(pendingReleases != nullptr)
  {
    for(int i = 0; i < NodeBufferSize; i++)
    {
      pendingReleases->buffer[i]->release();
    }
    AutoreleaseStructure *toDelete = pendingReleases;
    pendingReleases = pendingReleases->parent;
    delete toDelete;
  }
  insertLock.unlock();

  _currentPool = parent;

  // If this autoreleasepool is on the heap, draining should delete it.
  if(referenceCount() != -1)
  {
    delete this;
  }
}

