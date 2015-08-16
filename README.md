# ReferenceCountedCPP
Just a really quickly written refcounting base class to be inherited from

Any custom object you've written can enable reference counting by just
inheriting from ReferenceCounted.

It works very similar to manual reference counting in Objective-C.
Calling retain increases it's reference count
Calling release decreases it's reference count
When its reference count hits zero it will automatically be freed.

It can be used in a few different ways
  - You can compile it on your own and link it with your own source code that
  you want to use it in. #include "refcount.h" will give you access to
  everything

  - You can compile it into a static library called librefcount.a by running
  "make local". This can be used by #including "refcount.h" and linking it with 
  your project by using adding the following flags:
    -L(dir of librefcount.a) -lrefcount 

  - You can compile it into a dynamic library and install it on your system.
  This is the easiest way to use it but necessitates that all users have the
  library installed on their system. You can do this by running make install
  (and make uninstall to remove) and #include <refcount> anywhere it's used
