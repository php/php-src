Memory Management TODO

The Reference Counting page contained TODOs for dedicated Cycle Collector and
Zend Allocator pages; grouping them under Memory Management makes sense. Pages
to be added:

- Cycle Collector:
  candidate buffering, collection phases, collectable types, and correct use of
  GC flags and macros.
- Zend Allocator:
  allocator pairing, overflow-safe allocation, request/persistent lifetimes, and
  relevant arena cleanup.
