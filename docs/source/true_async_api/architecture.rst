##############
 Architecture
##############

The TrueAsync API follows a clean separation-of-concerns architecture that splits API definition
from implementation. This design enables modularity, testability, and support for multiple
backend implementations.

The **True Async API** is divided into three main functional blocks:

- **Scheduler**
  Manages coroutines and the main event loop.

- **Reactor**
  Handles everything related to input/output (I/O), including sockets, timers, and file descriptors.

- **Thread Pool**
  Allows execution of blocking or heavy tasks in a thread pool.

---

## Implementation Flexibility

The True Async API provides clearly separated interfaces for each block, allowing them to be implemented independently—even across different PHP extensions. For example:

- The Scheduler can be implemented as part of the PHP core.
- The Reactor can be implemented in a separate extension using `libuv`.
- The Thread Pool can be based on native threads or a library like `pthreads`.

This approach gives developers maximum flexibility and control over asynchronous execution.