#include <iostream>
#include <new>
#include <unordered_set>
#include "ReferenceCounted.h"


/* Logging */
#define shouldLog 1

#if shouldLog

#include <iostream>
#define staticDebugLog(string) if(shouldLog) {\
  std::cerr << "===== In static refcount operator: " << string << std::endl; \
}

#define debugLog(string) if(shouldLog && _referenceCount != -1) {\
std::cerr << "===== Reference counted " << this << ": " << \
  string << " =====" << std::endl; \
}

#else
#define staticDebugLog(string)
#define debugLog(string)
#endif

/* */

static std::mutex heapSetLock;
static std::unordered_set<void *> heapAddressSet;

ReferenceCounted::ReferenceCounted()
{
  heapSetLock.lock();
  _referenceCount = heapAddressSet.count(this) ? 1 : -1;
  heapAddressSet.erase(this);
  debugLog("Constructing a reference counted object");
  heapSetLock.unlock();
}

// Nothing needs to be done here but we need to make sure calling delete this in
// release will free the call down to the base class destructor
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
  refLock.lock();
  if(_referenceCount <= 1)
  {
    _referenceCount = 0;
    debugLog("Releasing object. Refcount dropped below 1. Will be releasing.");
    refLock.unlock();
    delete this;
  }
  else
  {
    _referenceCount--;
    debugLog("Releasing object. Refcount currently " << _referenceCount);
    refLock.unlock();
  }
}

void ReferenceCounted::autorelease()
{
  // Not yet implemented
  if(_referenceCount == -1)
  {
    throw std::runtime_error("Retain can only be called on dynamically allocated objects");
  }
  debugLog("Autoreleasing. Current refcount: " << _referenceCount);

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




