################
 TrueAsync API
################

.. toctree::
   :hidden:

   architecture
   events
   coroutines
   api-reference
   implementation-guide
   patterns
   examples

The TrueAsync API provides a comprehensive asynchronous programming interface for PHP extensions,
built on top of the Zend Engine. This API enables non-blocking operations for I/O, timers,
signals, and other asynchronous tasks while maintaining clean separation between the API
definition and its implementation.

.. warning::

   TrueAsync API is currently in development. The API may change in future versions.

The TrueAsync API follows a strict architectural pattern where the PHP core (php-src) defines
the API interfaces, while extensions provide concrete implementations. This design ensures
modularity, testability, and allows for multiple backend implementations (libuv, epoll, etc.).

******************
 Key Features
******************

* **Non-blocking I/O operations** - File, network, and socket operations
* **Timer and scheduling support** - Timeout handling and periodic tasks  
* **Signal handling** - Asynchronous signal processing
* **DNS resolution** - Async getaddrinfo/getnameinfo with proper memory management
* **Process management** - Spawn and monitor external processes
* **Filesystem monitoring** - Watch for file and directory changes
* **Clean separation** - API definition vs implementation
* **Memory safety** - Proper reference counting and cleanup

******************
 Architecture
******************

The TrueAsync API consists of two main components:

**API Definition Layer** (``Zend/zend_async_API.h``)
   Defines function pointer types, structures, and macros that form the public API.
   Located in the php-src core and used by all consumers.

**Implementation Layer** (``ext/async/``)
   Provides concrete implementations using libuv or other event loop libraries.
   Registers implementations with the core through registration functions.

This separation allows php-src core to use async functionality without directly depending
on any specific implementation, enabling clean modular design and testing.

******************
 Getting Started
******************

To use the TrueAsync API in your extension:

1. Include the API header: ``#include "Zend/zend_async_API.h"``
2. Check if async is enabled: ``ZEND_ASYNC_IS_ENABLED()``
3. Use API macros like ``ZEND_ASYNC_GETADDRINFO()`` for operations
4. Handle results through callback mechanisms
5. Clean up resources with appropriate free functions

For implementation details, see the :doc:`implementation-guide` section.

******************
 Quick Example
******************

.. code:: c

   // Async DNS resolution
   zend_async_dns_addrinfo_t *event = ZEND_ASYNC_GETADDRINFO("example.com", "80", &hints);
   
   // Register callback for completion
   zend_async_resume_when(coroutine, &event->base, true, my_callback, NULL);
   
   // Later, in callback: clean up
   ZEND_ASYNC_FREEADDRINFO(result);

******************
 Documentation Sections
******************

:doc:`architecture`
   Detailed explanation of the two-repository architecture and component separation.

:doc:`coroutines`
   Complete guide to coroutines, context switching, and internal context management.

:doc:`api-reference`
   Complete API reference with function signatures, macros, and data structures.

:doc:`implementation-guide`
   Guide for implementing TrueAsync API backends and registering with the core.

:doc:`patterns`
   Common patterns, best practices, and coding conventions for the API.

:doc:`examples`
   Practical examples showing how to use different API components.