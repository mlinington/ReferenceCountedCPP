# ReferenceCountedCPP
Just a really quickly written refcounting base class to be inherited from

Any custom object you've written can enable reference counting by just
inheriting from ReferenceCounted.

It works very similar to manual reference counting in Objective-C.
Calling retain increases it's reference count
Calling release decreases it's reference count
When its reference count hits zero it will automatically be freed.
