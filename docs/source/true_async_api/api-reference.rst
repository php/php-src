###############
 API Reference
###############

This section provides comprehensive documentation for the TrueAsync API functions, macros, and data
structures based on the actual implementation in ``Zend/zend_async_API.h``.

*************
 API Version
*************

.. code:: c

   #define ZEND_ASYNC_API "TrueAsync API v0.4.0"
   #define ZEND_ASYNC_API_VERSION_MAJOR 0
   #define ZEND_ASYNC_API_VERSION_MINOR 4
   #define ZEND_ASYNC_API_VERSION_PATCH 0

*****************
 Core API Macros
*****************

Enablement Check
================

.. code:: c

   ZEND_ASYNC_IS_ENABLED()

Returns ``true`` if async functionality is available, ``false`` otherwise. Always check this before
using any async APIs.

.. code:: c

   ZEND_ASYNC_REACTOR_IS_ENABLED()

Returns ``true`` if reactor (event loop) is available and functional.

**********************
 Coroutine Operations
**********************

Coroutine Creation
==================

.. code:: c

   ZEND_ASYNC_SPAWN()
   ZEND_ASYNC_SPAWN_WITH(scope)
   ZEND_ASYNC_SPAWN_WITH_PROVIDER(scope_provider)
   ZEND_ASYNC_SPAWN_WITH_PRIORITY(priority)
   ZEND_ASYNC_SPAWN_WITH_SCOPE_EX(scope, priority)

Creates and spawns new coroutines with various configuration options:

-  ``ZEND_ASYNC_SPAWN()`` - Create coroutine with default settings
-  ``ZEND_ASYNC_SPAWN_WITH(scope)`` - Create coroutine within specific scope
-  ``ZEND_ASYNC_SPAWN_WITH_PROVIDER(scope_provider)`` - Use scope provider object
-  ``ZEND_ASYNC_SPAWN_WITH_PRIORITY(priority)`` - Set coroutine priority
-  ``ZEND_ASYNC_SPAWN_WITH_SCOPE_EX(scope, priority)`` - Both scope and priority

**Priority levels:**

.. code:: c

   typedef enum {
       ZEND_COROUTINE_NORMAL = 0,
       ZEND_COROUTINE_HI_PRIORITY = 255
   } zend_coroutine_priority;

Coroutine Management
====================

.. code:: c

   ZEND_ASYNC_NEW_COROUTINE(scope)

Creates a new coroutine object without starting it.

.. code:: c

   ZEND_ASYNC_ENQUEUE_COROUTINE(coroutine)

Adds coroutine to execution queue for scheduling.

.. code:: c

   ZEND_ASYNC_SUSPEND()
   ZEND_ASYNC_RUN_SCHEDULER_AFTER_MAIN()

Suspends current coroutine execution:

-  ``SUSPEND()`` - Normal suspension
-  ``RUN_SCHEDULER_AFTER_MAIN()`` - Suspend from main context

Coroutine Control
=================

.. code:: c

   ZEND_ASYNC_RESUME(coroutine)
   ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, error, transfer_error)

Resume coroutine execution:

-  ``RESUME(coroutine)`` - Normal resume
-  ``RESUME_WITH_ERROR(coroutine, error, transfer_error)`` - Resume with exception

.. code:: c

   ZEND_ASYNC_CANCEL(coroutine, error, transfer_error)
   ZEND_ASYNC_CANCEL_EX(coroutine, error, transfer_error, is_safely)

Cancel coroutine execution:

-  ``CANCEL()`` - Cancel with default safety
-  ``CANCEL_EX()`` - Cancel with explicit safety flag

***********
 Scope API
***********

Scope Creation
==============

.. code:: c

   ZEND_ASYNC_NEW_SCOPE(parent)
   ZEND_ASYNC_NEW_SCOPE_WITH_OBJECT(parent)

Create new async scopes for coroutine isolation:

-  ``NEW_SCOPE(parent)`` - Create scope with parent reference
-  ``NEW_SCOPE_WITH_OBJECT(parent)`` - Create scope backed by zend_object

Scope Flags
===========

.. code:: c

   #define ZEND_ASYNC_SCOPE_IS_CLOSED(scope)
   #define ZEND_ASYNC_SCOPE_IS_CANCELLED(scope)
   #define ZEND_ASYNC_SCOPE_IS_DISPOSING(scope)

Check scope status:

-  ``IS_CLOSED`` - Scope has been closed
-  ``IS_CANCELLED`` - Scope was cancelled
-  ``IS_DISPOSING`` - Scope is being disposed

***********
 Event API
***********

Event Lifecycle
===============

All async events implement these core function pointers:

.. code:: c

   typedef void (*zend_async_event_start_t)(zend_async_event_t *event);
   typedef void (*zend_async_event_stop_t)(zend_async_event_t *event);
   typedef void (*zend_async_event_dispose_t)(zend_async_event_t *event);

Event State Checking
====================

.. code:: c

   ZEND_ASYNC_EVENT_IS_CLOSED(event)

Check if event has been closed.

Event Reference Counting
========================

.. code:: c

   ZEND_ASYNC_EVENT_REF(event)
   ZEND_ASYNC_EVENT_ADD_REF(event)
   ZEND_ASYNC_EVENT_DEL_REF(event)

Manage event reference counts for memory safety.

Event Flags
===========

.. code:: c

   #define ZEND_ASYNC_EVENT_F_CLOSED        (1u << 0)  /* event was closed */
   #define ZEND_ASYNC_EVENT_F_RESULT_USED   (1u << 1)  /* result will be used */
   #define ZEND_ASYNC_EVENT_F_EXC_CAUGHT    (1u << 2)  /* error was caught */
   #define ZEND_ASYNC_EVENT_F_ZVAL_RESULT   (1u << 3)  /* result is zval */

.. code:: c

   ZEND_ASYNC_EVENT_SET_CLOSED(event)
   ZEND_ASYNC_EVENT_CLR_CLOSED(event)

Manipulate event flags.

*************
 Reactor API
*************

Reactor Control
===============

.. code:: c

   ZEND_ASYNC_REACTOR_STARTUP()

Start the async reactor (event loop).

Poll Events
===========

.. code:: c

   typedef enum {
       ASYNC_READABLE = 1,
       ASYNC_WRITABLE = 2,
       ASYNC_DISCONNECT = 4,
       ASYNC_PRIORITIZED = 8
   } async_poll_event;

Event flags for socket polling operations.

******************
 Signal Constants
******************

.. code:: c

   #define ZEND_ASYNC_SIGHUP    1
   #define ZEND_ASYNC_SIGINT    2
   #define ZEND_ASYNC_SIGQUIT   3
   #define ZEND_ASYNC_SIGILL    4
   #define ZEND_ASYNC_SIGTERM   15
   #define ZEND_ASYNC_SIGBREAK  21
   #define ZEND_ASYNC_SIGABRT   22
   #define ZEND_ASYNC_SIGWINCH  28

Cross-platform signal constants for async signal handling.

*******************
 Process Execution
*******************

Execution Modes
===============

.. code:: c

   typedef enum {
       ZEND_ASYNC_EXEC_MODE_EXEC,        /* exec() - last line only */
       ZEND_ASYNC_EXEC_MODE_SYSTEM,      /* system() - print + return last */
       ZEND_ASYNC_EXEC_MODE_EXEC_ARRAY,  /* exec() with array output */
       ZEND_ASYNC_EXEC_MODE_PASSTHRU,    /* passthru() - binary output */
       ZEND_ASYNC_EXEC_MODE_SHELL_EXEC   /* shell_exec() - buffer output */
   } zend_async_exec_mode;

************************
 Future and Channel API
************************

Futures
=======

.. code:: c

   ZEND_ASYNC_NEW_FUTURE(thread_safe)
   ZEND_ASYNC_NEW_FUTURE_EX(thread_safe, extra_size)

Create future objects for async result handling.

Channels
========

.. code:: c

   ZEND_ASYNC_NEW_CHANNEL(buffer_size, resizable, thread_safe)
   ZEND_ASYNC_NEW_CHANNEL_EX(buffer_size, resizable, thread_safe, extra_size)

Create channels for coroutine communication.

*****************************
 API Context Switch Handlers
*****************************

Registration
============

.. code:: c

   uint32_t zend_coroutine_add_switch_handler(
       zend_coroutine_t *coroutine,
       zend_coroutine_switch_handler_fn handler
   );

   bool zend_coroutine_remove_switch_handler(
       zend_coroutine_t *coroutine,
       uint32_t handler_index
   );

Add/remove context switch handlers for individual coroutines.

Global Handlers
===============

.. code:: c

   void zend_async_add_main_coroutine_start_handler(
       zend_coroutine_switch_handler_fn handler
   );

Add global handlers that are called when main coroutines start.

Handler Function Type
=====================

.. code:: c

   typedef bool (*zend_coroutine_switch_handler_fn)(
       zend_coroutine_t *coroutine,
       bool is_enter,     /* true = entering, false = leaving */
       bool is_finishing  /* true = coroutine finishing */
   );

Returns ``true`` to keep handler, ``false`` to remove it after execution.

*******************
 Utility Functions
*******************

System Management
=================

.. code:: c

   ZEND_ASYNC_SHUTDOWN()
   ZEND_ASYNC_ENGINE_SHUTDOWN()

Initiate graceful shutdown of async subsystem.

.. code:: c

   ZEND_ASYNC_GET_COROUTINES()
   ZEND_ASYNC_GET_AWAITING_INFO(coroutine)

Get information about active coroutines and their await status.

Microtasks
==========

.. code:: c

   ZEND_ASYNC_ADD_MICROTASK(microtask)

Add microtask to execution queue.

Exception Handling
==================

.. code:: c

   ZEND_ASYNC_SPAWN_AND_THROW(exception, scope, priority)

Spawn coroutine specifically for throwing an exception within a scope.

**********************
 API Error Management
**********************

API Exception Classes
=====================

.. code:: c

   typedef enum {
       ZEND_ASYNC_CLASS_NO = 0,
       ZEND_ASYNC_CLASS_AWAITABLE = 1,
       ZEND_ASYNC_CLASS_COROUTINE = 2
   } zend_async_class;

   ZEND_ASYNC_GET_CE(type)
   ZEND_ASYNC_GET_EXCEPTION_CE(type)

Get class entries for async-related classes and exceptions.

************
 Data Types
************

Platform Types
==============

.. code:: c

   #ifdef PHP_WIN32
   typedef HANDLE zend_file_descriptor_t;
   typedef DWORD zend_process_id_t;
   typedef HANDLE zend_process_t;
   typedef SOCKET zend_socket_t;
   #else
   typedef int zend_file_descriptor_t;
   typedef pid_t zend_process_id_t;
   typedef pid_t zend_process_t;
   typedef int zend_socket_t;
   #endif

Cross-platform type definitions for file descriptors, processes, and sockets.

********************
 API Best Practices
********************

#. **Always check enablement** with ``ZEND_ASYNC_IS_ENABLED()`` before using APIs
#. **Use appropriate scopes** for coroutine isolation
#. **Handle reference counting** properly with event objects
#. **Register cleanup handlers** using context switch handlers
#. **Check reactor availability** before using event-based operations
#. **Use priority levels** appropriately for coroutine scheduling
