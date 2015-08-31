#include "refcount.h"

#include <unordered_set>

// set true/false to enable/disable logging
#define debug false
#if debug 
#include <iostream>
#define staticDebugLog(string) std::cerr << "===== In static refcount operator " \
  << string << " =====" << std::endl;

#define debugLog(string) if(_referenceCount != -1) {\
std::cerr << "===== Reference counted " << this << ": " << \
  string << " =====" << std::endl; \
}

#else
#define staticDebugLog(string)
#define debugLog(string)
#endif

/* */

// Used to tell if an object is allocated on stack or heap
static std::mutex heapSetLock;
static std::unordered_set<void *> heapAddressSet;

ReferenceCounted::ReferenceCounted()
{
  heapSetLock.lock();

  _referenceCount = heapAddressSet.count(this) ? 1 : -1;
  heapAddressSet.erase(this);
  debugLog("Constructed a reference counted object");

  heapSetLock.unlock();
}

// Nothing needs to be done here but we need to make sure calling "delete this" 
// in release will free the call down to the base class destructor
ReferenceCounted::~ReferenceCounted() 
{
  debugLog("Destructing reference counted object");
}

void ReferenceCounted::retain()
{
  if(_referenceCount == -1)
  {
    throw std::runtime_error("Retain can only be called on dynamically allocated objects");
  }

  refLock.lock();

  _referenceCount++;
  debugLog("Retaining, now has reference count: " << _referenceCount);

  refLock.unlock();
}

void ReferenceCounted::release()
{
  if(_referenceCount == -1)
  {
    throw std::runtime_error("Retain can only be called on dynamically allocated objects");
  }
#if debug
  // this will segfault once it hits delete this
  else if(_referenceCount == 0)
  {
    throw std::runtime_error("Item has already been deleted");
  }
#endif

  refLock.lock();

  if(_referenceCount <= 1)
  {
    _referenceCount = 0;
    debugLog("Releasing. Current refcount: " << _referenceCount);
    refLock.unlock();
    delete this;
  }
  else
  {
    _referenceCount--;
    debugLog("Releasing. Current refcount: " << _referenceCount);
    refLock.unlock();
  }
}

void ReferenceCounted::autorelease()
{
  if(_referenceCount == -1)
  {
    throw std::runtime_error("Autorelease can only be called on dynamically allocated objects");
  }
  debugLog("Autoreleasing. Current refcount: " << _referenceCount);

  AutoreleasePool *currentPool = AutoreleasePool::currentPool();
  if(!currentPool)
  {
    throw std::runtime_error("No autorelasepool in the current thread");
  }
  currentPool->addObject(this);
}

int ReferenceCounted::referenceCount()
{
  return _referenceCount;
}

void *ReferenceCounted::operator new(size_t size)
{
  staticDebugLog("New operator hit");
  void *returnAddr = malloc(size);
  if(!returnAddr)
  {
    throw std::bad_alloc();
  }

  heapSetLock.lock();
  heapAddressSet.insert(returnAddr);
  heapSetLock.unlock();
  return returnAddr;
}

void ReferenceCounted::operator delete(void *addr)
{
  staticDebugLog("Delete operator hit");
  free(addr);
}



