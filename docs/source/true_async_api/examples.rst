##########
 Examples
##########

This section provides practical C code examples for using the TrueAsync API in PHP extensions.

****************
 Basic Examples
****************

Checking API Availability
=========================

Always check if async is enabled before using the API:

.. code:: c

   #include "Zend/zend_async_API.h"

   void my_async_function(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) {
           // Fallback to synchronous operation
           my_sync_function();
           return;
       }

       // Use async API
       my_actual_async_function();
   }

Simple Coroutine Creation
=========================

Creating and spawning a basic coroutine:

.. code:: c

   static void my_coroutine_entry(void *arg)
   {
       php_printf("Hello from coroutine!\n");
   }

   void spawn_example(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) return;

       zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
       if (!coro) {
           php_error(E_WARNING, "Failed to create coroutine");
           return;
       }

       coro->internal_entry = my_coroutine_entry;
       coro->internal_arg = NULL;

       ZEND_ASYNC_ENQUEUE_COROUTINE(coro);
   }

Using Scopes
============

Creating coroutines with custom scopes:

.. code:: c

   void scope_example(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) return;

       // Create new scope
       zend_async_scope_t *scope = ZEND_ASYNC_NEW_SCOPE(NULL);
       if (!scope) return;

       // Spawn coroutine within scope
       zend_coroutine_t *coro = ZEND_ASYNC_SPAWN_WITH(scope);
       if (coro) {
           coro->internal_entry = my_coroutine_entry;
           ZEND_ASYNC_ENQUEUE_COROUTINE(coro);
       }
   }

*********************************
 Context Switch Handler Examples
*********************************

Per-Coroutine Handler
=====================

Adding context switch handlers to specific coroutines:

.. code:: c

   static bool my_switch_handler(zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (is_finishing) {
           php_printf("Coroutine finishing\n");
           return false; // Remove handler
       }

       if (is_enter) {
           php_printf("Entering coroutine\n");
       } else {
           php_printf("Leaving coroutine\n");
       }

       return true; // Keep handler
   }

   void add_handler_example(void)
   {
       zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
       if (!coro) return;

       uint32_t handler_id = zend_coroutine_add_switch_handler(coro, my_switch_handler);

       coro->internal_entry = my_coroutine_entry;
       ZEND_ASYNC_ENQUEUE_COROUTINE(coro);

       // Later: remove handler if needed
       // zend_coroutine_remove_switch_handler(coro, handler_id);
   }

Global Main Coroutine Handler
=============================

Handler that applies to all main coroutines:

.. code:: c

   static uint32_t my_context_key = 0;

   static bool main_coroutine_handler(zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (!is_enter) return true;

       // Initialize extension data for this coroutine
       my_extension_data_t *data = ecalloc(1, sizeof(my_extension_data_t));
       data->initialized = true;

       // Store in coroutine internal context
       zval data_zval;
       ZVAL_PTR(&data_zval, data);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, my_context_key, &data_zval);

       return false; // Remove handler after initialization
   }

   PHP_MINIT_FUNCTION(my_extension)
   {
       my_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("my_extension");
       zend_async_add_main_coroutine_start_handler(main_coroutine_handler);
       return SUCCESS;
   }

******************
 Event Operations
******************

Basic Event Usage
=================

Working with async events (using timer as example):

.. code:: c

   static void timer_callback(zend_async_event_t *event, zend_async_event_callback_t *callback,
                             void *result, zend_object *exception)
   {
       if (exception) {
           php_printf("Timer error occurred\n");
       } else {
           php_printf("Timer fired!\n");
       }

       // Clean up
       ZEND_ASYNC_EVENT_DEL_REF(event);
   }

   void timer_example(void)
   {
       if (!ZEND_ASYNC_REACTOR_IS_ENABLED()) return;

       // This would need actual reactor implementation
       // zend_async_timer_event_t *timer = zend_async_new_timer_event_fn(1000, 0, false, 0);
       // if (!timer) return;

       // zend_async_event_callback_t *callback = ...;
       // timer->base.add_callback(&timer->base, callback);
       // timer->base.start(&timer->base);
   }

***************************
 Internal Context Examples
***************************

Storing Extension Data
======================

Using internal context to store per-coroutine data:

.. code:: c

   static uint32_t cache_context_key = 0;

   typedef struct {
       HashTable *cached_results;
       time_t last_access;
   } cache_data_t;

   PHP_MINIT_FUNCTION(cache_extension)
   {
       cache_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("cache_data");
       return SUCCESS;
   }

   cache_data_t* get_cache_data(zend_coroutine_t *coroutine)
   {
       zval *cached = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, cache_context_key);
       if (cached && Z_TYPE_P(cached) == IS_PTR) {
           return (cache_data_t *)Z_PTR_P(cached);
       }

       // Create new cache data
       cache_data_t *data = ecalloc(1, sizeof(cache_data_t));
       ALLOC_HASHTABLE(data->cached_results);
       zend_hash_init(data->cached_results, 16, NULL, ZVAL_PTR_DTOR, 0);
       data->last_access = time(NULL);

       // Store in context
       zval data_zval;
       ZVAL_PTR(&data_zval, data);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, cache_context_key, &data_zval);

       return data;
   }

*************************
 Error Handling Examples
*************************

Exception Creation
==================

Creating and handling exceptions in async context:

.. code:: c

   void async_operation_with_error(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) return;

       zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
       if (!coro) {
           zend_throw_error(NULL, "Failed to create coroutine");
           return;
       }

       // Set up coroutine that might throw
       coro->internal_entry = error_prone_operation;
       ZEND_ASYNC_ENQUEUE_COROUTINE(coro);
   }

   static void error_prone_operation(void *arg)
   {
       // Simulate error condition
       if (some_error_condition()) {
           zend_object *exception = zend_throw_exception(
               zend_ce_exception, "Async operation failed", 0
           );

           // Resume current coroutine with error
           zend_coroutine_t *current = ZEND_ASYNC_CURRENT_COROUTINE;
           if (current) {
               ZEND_ASYNC_RESUME_WITH_ERROR(current, exception, true);
           }
           return;
       }

       php_printf("Operation completed successfully\n");
   }

****************************
 Memory Management Examples
****************************

Reference Counting Examples
===========================

Proper event reference management:

.. code:: c

   void event_reference_example(void)
   {
       // Assuming we have an event from somewhere
       zend_async_event_t *event = get_some_event();

       // Add reference when storing event
       ZEND_ASYNC_EVENT_ADD_REF(event);

       // Store event somewhere
       store_event_reference(event);

       // Later: remove reference when done
       ZEND_ASYNC_EVENT_DEL_REF(event);

       // Or use RELEASE which decrements and disposes if needed
       // ZEND_ASYNC_EVENT_RELEASE(event);
   }

*******************************
 Futures and Channels Examples
*******************************

Future Creation
===============

Creating and using futures for async results:

.. code:: c

   void future_example(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) return;

       // Create thread-safe future
       zend_future_t *future = ZEND_ASYNC_NEW_FUTURE(true);
       if (!future) return;

       // Future can be resolved from any thread
       // zend_future_resolve(future, &result_zval);

       // Or with exception
       // zend_future_reject(future, exception);
   }

Channel Communication
=====================

Creating channels for coroutine communication:

.. code:: c

   void channel_example(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) return;

       // Create buffered channel
       zend_async_channel_t *channel = ZEND_ASYNC_NEW_CHANNEL(10, true, false);
       if (!channel) return;

       // Channels support send/receive operations
       // channel->send(channel, &value);
       // channel->receive(channel, &result);
   }

***********************
 Extension Integration
***********************

Complete Extension Example
==========================

Full example of async-aware extension:

.. code:: c

   static uint32_t my_ext_context_key = 0;

   typedef struct {
       bool async_enabled;
       HashTable *pending_operations;
   } my_extension_context_t;

   static bool my_ext_main_handler(zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (is_finishing) {
           // Cleanup on coroutine completion
           zval *ctx_zval = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, my_ext_context_key);
           if (ctx_zval && Z_TYPE_P(ctx_zval) == IS_PTR) {
               my_extension_context_t *ctx = (my_extension_context_t *)Z_PTR_P(ctx_zval);
               if (ctx->pending_operations) {
                   zend_hash_destroy(ctx->pending_operations);
                   FREE_HASHTABLE(ctx->pending_operations);
               }
               efree(ctx);
           }
           return false;
       }

       if (!is_enter) return true;

       // Initialize context for new main coroutine
       my_extension_context_t *ctx = ecalloc(1, sizeof(my_extension_context_t));
       ctx->async_enabled = true;

       ALLOC_HASHTABLE(ctx->pending_operations);
       zend_hash_init(ctx->pending_operations, 8, NULL, ZVAL_PTR_DTOR, 0);

       zval ctx_zval;
       ZVAL_PTR(&ctx_zval, ctx);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, my_ext_context_key, &ctx_zval);

       return false; // Remove handler after init
   }

   PHP_MINIT_FUNCTION(my_extension)
   {
       my_ext_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("my_extension");

       if (ZEND_ASYNC_IS_ENABLED()) {
           zend_async_add_main_coroutine_start_handler(my_ext_main_handler);
       }

       return SUCCESS;
   }

   // Extension function that works async-aware
   PHP_FUNCTION(my_async_operation)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) {
           // Fallback to sync operation
           my_sync_operation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
           return;
       }

       zend_coroutine_t *current = ZEND_ASYNC_CURRENT_COROUTINE;
       if (current) {
           my_extension_context_t *ctx = get_my_context(current);
           if (ctx && ctx->async_enabled) {
               // Perform async operation
               perform_async_operation(current);
               return;
           }
       }

       // Default to sync
       my_sync_operation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
   }

*************************
 Examples Best Practices
*************************

#. **Always check enablement** before using async APIs
#. **Use internal context** for per-coroutine data storage
#. **Handle reference counting** properly for events
#. **Register cleanup handlers** for resource management
#. **Provide sync fallbacks** when async is not available
#. **Use appropriate scopes** for coroutine isolation
#. **Handle exceptions** properly in async context
