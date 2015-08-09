#ifndef _REFERENCECOUNTED
#define _REFERENCECOUNTED

#include <mutex>

class ReferenceCounted
{
  private:
    int _referenceCount;
    std::mutex refLock;

  public:
    ReferenceCounted();
    virtual ~ReferenceCounted();

    void retain();
    void release();
    void autorelease();

    int referenceCount();

    void *operator new(size_t size);
    void operator delete(void *addr);
};

#endif
