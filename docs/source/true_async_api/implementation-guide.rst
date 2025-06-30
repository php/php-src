######################
 Implementation Guide
######################

This guide explains how to implement TrueAsync API backends and integrate them with
the PHP core. It covers the registration process, implementation patterns, and best practices.

**********************
 Overview
**********************

Implementing a TrueAsync API backend involves:

1. **Creating function implementations** that match the API signatures
2. **Registering implementations** with the core during module initialization
3. **Following memory management** and error handling conventions  
4. **Implementing event lifecycle** methods (start, stop, dispose)
5. **Handling callbacks** and result delivery

***********************
 Registration Process
***********************

The registration process connects your implementations to the core API.

Module Initialization
=====================

Register your implementations during ``PHP_MINIT_FUNCTION``:

.. code:: c

   PHP_MINIT_FUNCTION(my_async_backend) {
       // Register reactor implementations
       bool success = zend_async_reactor_register(
           "MyAsyncBackend",           // Module name
           false,                      // Allow override
           my_reactor_startup,         // Startup function
           my_reactor_shutdown,        // Shutdown function  
           my_reactor_execute,         // Execute function
           my_reactor_loop_alive,      // Loop alive check
           my_new_socket_event,        // Socket event factory
           my_new_poll_event,          // Poll event factory
           my_new_timer_event,         // Timer event factory
           my_new_signal_event,        // Signal event factory
           my_new_process_event,       // Process event factory
           my_new_thread_event,        // Thread event factory
           my_new_filesystem_event,    // Filesystem event factory
           my_getnameinfo,             // DNS nameinfo
           my_getaddrinfo,             // DNS addrinfo
           my_freeaddrinfo,            // DNS cleanup
           my_new_exec_event,          // Exec event factory
           my_exec                     // Exec function
       );
       
       if (!success) {
           php_error_docref(NULL, E_CORE_ERROR, "Failed to register async backend");
           return FAILURE;
       }
       
       return SUCCESS;
   }

Function Pointer Types
======================

All implementations must match the exact function pointer signatures defined in
``zend_async_API.h``:

.. code:: c

   // DNS function signatures
   typedef zend_async_dns_addrinfo_t* (*zend_async_getaddrinfo_t)(
       const char *node, const char *service, 
       const struct addrinfo *hints, size_t extra_size
   );
   
   typedef void (*zend_async_freeaddrinfo_t)(struct addrinfo *ai);
   
   // Timer function signature
   typedef zend_async_timer_event_t* (*zend_async_new_timer_event_t)(
       const zend_ulong timeout, const zend_ulong nanoseconds,
       const bool is_periodic, size_t extra_size
   );

***********************
 Implementation Patterns
***********************

Event Structure Pattern
=======================

All async events follow a common pattern with base event structure plus
implementation-specific data:

.. code:: c

   // Your implementation structure
   typedef struct _my_dns_addrinfo_s {
       zend_async_dns_addrinfo_t event;  // Base event (MUST be first)
       my_event_loop_handle_t handle;    // Your event loop handle
       // Additional implementation-specific fields...
   } my_dns_addrinfo_t;

Factory Function Pattern
========================

Event factory functions create and initialize event structures:

.. code:: c

   static zend_async_dns_addrinfo_t* my_getaddrinfo(
       const char *node, const char *service,
       const struct addrinfo *hints, size_t extra_size
   ) {
       // Allocate memory (include extra_size if specified)
       size_t total_size = sizeof(my_dns_addrinfo_t) + extra_size;
       my_dns_addrinfo_t *dns = pecalloc(1, total_size, 0);
       
       // Initialize base event fields
       dns->event.base.extra_offset = sizeof(my_dns_addrinfo_t);
       dns->event.base.ref_count = 1;
       dns->event.base.add_callback = my_add_callback;
       dns->event.base.del_callback = my_remove_callback;
       dns->event.base.start = my_dns_start;
       dns->event.base.stop = my_dns_stop;
       dns->event.base.dispose = my_dns_dispose;
       
       // Store request parameters
       dns->event.node = node;
       dns->event.service = service;
       dns->event.hints = hints;
       
       // Initialize your backend-specific handle
       int result = my_event_loop_init_dns(&dns->handle, node, service, hints);
       if (result != 0) {
           my_throw_error("Failed to initialize DNS request: %s", my_strerror(result));
           pefree(dns, 0);
           return NULL;
       }
       
       // Link handle to event data
       dns->handle.data = dns;
       
       return &dns->event;
   }

Lifecycle Methods
=================

Implement the three lifecycle methods for each event type:

.. code:: c

   // Start the async operation
   static void my_dns_start(zend_async_event_t *event) {
       EVENT_START_PROLOGUE(event);  // Standard prologue
       
       my_dns_addrinfo_t *dns = (my_dns_addrinfo_t *)event;
       
       int result = my_event_loop_start_dns(&dns->handle, my_dns_callback);
       if (result != 0) {
           my_throw_error("Failed to start DNS operation: %s", my_strerror(result));
           return;
       }
       
       event->loop_ref_count++;
       ZEND_ASYNC_INCREASE_EVENT_COUNT;
   }
   
   // Stop the async operation  
   static void my_dns_stop(zend_async_event_t *event) {
       EVENT_STOP_PROLOGUE(event);  // Standard prologue
       
       my_dns_addrinfo_t *dns = (my_dns_addrinfo_t *)event;
       
       my_event_loop_stop_dns(&dns->handle);
       
       event->loop_ref_count = 0;
       ZEND_ASYNC_DECREASE_EVENT_COUNT;
   }
   
   // Clean up and dispose
   static void my_dns_dispose(zend_async_event_t *event) {
       if (ZEND_ASYNC_EVENT_REF(event) > 1) {
           ZEND_ASYNC_EVENT_DEL_REF(event);
           return;
       }
       
       // Stop if still running
       if (event->loop_ref_count > 0) {
           event->loop_ref_count = 1;
           event->stop(event);
       }
       
       // Free callbacks
       zend_async_callbacks_free(event);
       
       my_dns_addrinfo_t *dns = (my_dns_addrinfo_t *)event;
       
       // Clean up backend-specific resources
       my_event_loop_cleanup_dns(&dns->handle);
       
       // Free the structure
       pefree(dns, 0);
   }

********************
 Callback Handling
********************

Event Completion Callbacks
===========================

When your backend completes an operation, notify the core through callbacks:

.. code:: c

   // Your backend callback (called by event loop)
   static void my_dns_callback(my_event_loop_handle_t *handle, int status, struct addrinfo *result) {
       my_dns_addrinfo_t *dns = (my_dns_addrinfo_t *)handle->data;
       zend_object *exception = NULL;
       
       // Close the event after one-time operations
       ZEND_ASYNC_EVENT_SET_CLOSED(&dns->event.base);
       dns->event.base.stop(&dns->event.base);
       
       // Handle errors
       if (status != 0) {
           exception = my_new_exception(
               "DNS resolution failed: %s", my_strerror(status)
           );
       }
       
       // Notify all registered callbacks
       zend_async_callbacks_notify(&dns->event.base, result, exception);
       
       // Release exception if created
       if (exception) {
           zend_object_release(exception);
       }
   }

Result Memory Management
========================

For operations that return allocated memory (like ``getaddrinfo``):

.. code:: c

   static void my_freeaddrinfo(struct addrinfo *ai) {
       if (ai != NULL) {
           my_event_loop_free_addrinfo(ai);  // Use your backend's free function
       }
   }

**Make sure your ``getaddrinfo`` results can be freed by your ``freeaddrinfo`` implementation.**

*******************
 Error Handling
*******************

Error Reporting
===============

Use the standard PHP error reporting mechanisms:

.. code:: c

   // For recoverable errors during operation
   static void my_throw_error(const char *format, ...) {
       va_list args;
       va_start(args, format);
       zend_string *message = zend_vstrpprintf(0, format, args);
       va_end(args);
       
       zend_throw_error(NULL, ZSTR_VAL(message));
       zend_string_release(message);
   }
   
   // For creating exception objects
   static zend_object* my_new_exception(const char *format, ...) {
       va_list args;
       va_start(args, format);
       zend_string *message = zend_vstrpprintf(0, format, args);
       va_end(args);
       
       zend_object *ex = zend_throw_exception(zend_ce_exception, ZSTR_VAL(message), 0);
       zend_string_release(message);
       return ex;
   }

Exception Classes
=================

Use appropriate exception classes for different error types:

.. code:: c

   // DNS errors
   zend_object *dns_ex = async_new_exception(async_ce_dns_exception, "DNS error: %s", error_msg);
   
   // I/O errors  
   zend_object *io_ex = async_new_exception(async_ce_input_output_exception, "I/O error: %s", error_msg);
   
   // Timeout errors
   zend_object *timeout_ex = async_new_exception(async_ce_timeout_exception, "Operation timed out");

*******************
 Memory Management
*******************

Reference Counting
==================

The TrueAsync API uses reference counting for event objects:

.. code:: c

   // Add reference (when storing event somewhere)
   ZEND_ASYNC_EVENT_ADD_REF(event);
   
   // Remove reference (when done with event)
   ZEND_ASYNC_EVENT_DEL_REF(event);
   
   // Release event (decrements and disposes if needed)
   ZEND_ASYNC_EVENT_RELEASE(event);

Memory Allocation
=================

Use persistent allocation for long-lived structures:

.. code:: c

   // For event structures that may outlive a request
   my_event_t *event = pecalloc(1, sizeof(my_event_t), 0);  // Persistent
   
   // For temporary data
   char *buffer = emalloc(size);  // Request-scoped

Extra Data Support
==================

Support the ``extra_size`` parameter for extensions:

.. code:: c

   static zend_async_timer_event_t* my_new_timer_event(
       const zend_ulong timeout, const zend_ulong nanoseconds,
       const bool is_periodic, size_t extra_size
   ) {
       // Allocate base size + extra space
       size_t total_size = sizeof(my_timer_event_t) + extra_size;
       my_timer_event_t *timer = pecalloc(1, total_size, 0);
       
       // Set extra offset so extensions can find their data
       timer->event.base.extra_offset = sizeof(my_timer_event_t);
       
       // ... rest of initialization
   }

*******************
 Best Practices
*******************

Performance Considerations
==========================

* **Minimize allocations** in hot paths (event creation/callback handling)
* **Use object pools** for frequently created/destroyed events
* **Batch operations** when possible in your event loop
* **Profile memory usage** to detect leaks early

Thread Safety
==============

* **Document thread safety** guarantees of your implementation
* **Use proper locking** if supporting multi-threaded access
* **Consider thread-local storage** for per-thread state

Testing
=======

* **Create unit tests** for each API function implementation  
* **Test error conditions** and memory cleanup
* **Verify callback behavior** under various scenarios
* **Load test** with many concurrent operations

Documentation
=============

* **Document implementation-specific behavior** and limitations
* **Provide configuration options** for tuning performance
* **Include examples** showing integration with your event loop
* **Document version compatibility** with different backend versions