##########
 Patterns
##########

This section covers common patterns and best practices for implementing TrueAsync API in PHP
extensions.

**********************
 Fundamental Patterns
**********************

Async-Aware Function Pattern
============================

The standard pattern for making extension functions async-aware:

.. code:: c

   PHP_FUNCTION(my_extension_function)
   {
       // 1. Check if async is enabled
       if (!ZEND_ASYNC_IS_ENABLED()) {
           // Fallback to synchronous implementation
           my_sync_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
           return;
       }

       // 2. Check if we're in coroutine context
       zend_coroutine_t *current = ZEND_ASYNC_CURRENT_COROUTINE;
       if (!current) {
           // Not in coroutine, use sync
           my_sync_implementation(INTERNAL_FUNCTION_PARAM_PASSTHRU);
           return;
       }

       // 3. Perform async operation
       my_async_implementation(current, INTERNAL_FUNCTION_PARAM_PASSTHRU);
   }

API Enablement Check Pattern
============================

Always check API availability before using async features:

.. code:: c

   void my_extension_init(void)
   {
       if (ZEND_ASYNC_IS_ENABLED()) {
           // Initialize async-specific resources
           init_async_resources();

           if (ZEND_ASYNC_REACTOR_IS_ENABLED()) {
               // Reactor is available, can use events
               init_event_handlers();
           }
       }
   }

*****************************
 Context Management Patterns
*****************************

Extension Context Initialization
================================

Pattern for setting up per-coroutine extension data:

.. code:: c

   static uint32_t my_context_key = 0;

   typedef struct {
       HashTable *cache;
       zend_async_timer_event_t *cleanup_timer;
       bool initialized;
   } my_context_t;

   static bool my_context_initializer(zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (is_finishing) {
           // Cleanup context
           zval *ctx_zval = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, my_context_key);
           if (ctx_zval && Z_TYPE_P(ctx_zval) == IS_PTR) {
               my_context_t *ctx = (my_context_t *)Z_PTR_P(ctx_zval);
               cleanup_my_context(ctx);
           }
           return false; // Remove handler
       }

       if (!is_enter) return true;

       // Initialize context
       my_context_t *ctx = ecalloc(1, sizeof(my_context_t));
       ALLOC_HASHTABLE(ctx->cache);
       zend_hash_init(ctx->cache, 16, NULL, ZVAL_PTR_DTOR, 0);
       ctx->initialized = true;

       zval ctx_zval;
       ZVAL_PTR(&ctx_zval, ctx);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, my_context_key, &ctx_zval);

       return false; // One-time initialization
   }

   PHP_MINIT_FUNCTION(my_extension)
   {
       my_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("my_extension");

       if (ZEND_ASYNC_IS_ENABLED()) {
           zend_async_add_main_coroutine_start_handler(my_context_initializer);
       }

       return SUCCESS;
   }

Context Access Pattern
======================

Safe pattern for accessing extension context:

.. code:: c

   my_context_t* get_my_context(zend_coroutine_t *coroutine)
   {
       if (!coroutine) {
           coroutine = ZEND_ASYNC_CURRENT_COROUTINE;
           if (!coroutine) return NULL;
       }

       zval *ctx_zval = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, my_context_key);
       if (!ctx_zval || Z_TYPE_P(ctx_zval) != IS_PTR) {
           return NULL;
       }

       my_context_t *ctx = (my_context_t *)Z_PTR_P(ctx_zval);
       return ctx->initialized ? ctx : NULL;
   }

******************************
 Resource Management Patterns
******************************

RAII Pattern for Events
=======================

Resource Acquisition Is Initialization pattern for async events:

.. code:: c

   typedef struct {
       zend_async_event_t *event;
       bool owns_reference;
   } event_guard_t;

   static event_guard_t* event_guard_create(zend_async_event_t *event, bool add_ref)
   {
       event_guard_t *guard = emalloc(sizeof(event_guard_t));
       guard->event = event;
       guard->owns_reference = add_ref;

       if (add_ref) {
           ZEND_ASYNC_EVENT_ADD_REF(event);
       }

       return guard;
   }

   static void event_guard_destroy(event_guard_t *guard)
   {
       if (guard->owns_reference && guard->event) {
           ZEND_ASYNC_EVENT_DEL_REF(guard->event);
       }
       efree(guard);
   }

   void my_operation_with_event(void)
   {
       zend_async_event_t *event = create_some_event();
       event_guard_t *guard = event_guard_create(event, true);

       // Use event...

       // Automatic cleanup
       event_guard_destroy(guard);
   }

Cleanup Callback Pattern
========================

Registering cleanup callbacks for async operations:

.. code:: c

   typedef struct {
       void *user_data;
       void (*cleanup_fn)(void *data);
   } cleanup_context_t;

   static void cleanup_callback(zend_async_event_t *event, zend_async_event_callback_t *callback,
                               void *result, zend_object *exception)
   {
       cleanup_context_t *ctx = (cleanup_context_t *)callback->arg;

       if (ctx->cleanup_fn && ctx->user_data) {
           ctx->cleanup_fn(ctx->user_data);
       }

       efree(ctx);
   }

   void register_cleanup(zend_async_event_t *event, void *data, void (*cleanup_fn)(void*))
   {
       cleanup_context_t *ctx = emalloc(sizeof(cleanup_context_t));
       ctx->user_data = data;
       ctx->cleanup_fn = cleanup_fn;

       zend_async_event_callback_t *callback = zend_async_event_callback_new(
           event, cleanup_callback, 0
       );
       callback->arg = ctx;

       event->add_callback(event, callback);
   }

*************************
 Error Handling Patterns
*************************

Exception Propagation Pattern
=============================

Proper exception handling in async context:

.. code:: c

   static void async_operation_callback(zend_async_event_t *event, zend_async_event_callback_t *callback,
                                       void *result, zend_object *exception)
   {
       zend_coroutine_t *coroutine = (zend_coroutine_t *)callback->arg;

       if (exception) {
           // Propagate exception to coroutine
           ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, exception, true);
       } else {
           // Process result and resume normally
           process_async_result(result);
           ZEND_ASYNC_RESUME(coroutine);
       }

       // Cleanup
       ZEND_ASYNC_EVENT_DEL_REF(event);
   }

Error Recovery Pattern
======================

Handling recoverable errors in async operations:

.. code:: c

   typedef struct {
       int retry_count;
       int max_retries;
       void *operation_data;
   } retry_context_t;

   static void retry_operation_callback(zend_async_event_t *event, zend_async_event_callback_t *callback,
                                       void *result, zend_object *exception)
   {
       retry_context_t *ctx = (retry_context_t *)callback->arg;

       if (exception && ctx->retry_count < ctx->max_retries) {
           // Retry the operation
           ctx->retry_count++;
           php_error(E_NOTICE, "Retrying operation (attempt %d/%d)",
                     ctx->retry_count, ctx->max_retries);

           retry_async_operation(ctx);
           return;
       }

       // Either success or max retries reached
       if (exception) {
           handle_final_error(exception, ctx);
       } else {
           handle_success(result, ctx);
       }

       efree(ctx);
   }

**********************
 Performance Patterns
**********************

Object Pooling Pattern
======================

Reusing objects to reduce allocation overhead:

.. code:: c

   static zend_async_event_callback_t *callback_pool[MAX_POOL_SIZE];
   static int pool_size = 0;
   static zend_mutex_t pool_mutex;

   static zend_async_event_callback_t* get_pooled_callback(void)
   {
       zend_mutex_lock(&pool_mutex);

       zend_async_event_callback_t *callback = NULL;
       if (pool_size > 0) {
           callback = callback_pool[--pool_size];
       }

       zend_mutex_unlock(&pool_mutex);

       if (!callback) {
           callback = emalloc(sizeof(zend_async_event_callback_t));
           memset(callback, 0, sizeof(zend_async_event_callback_t));
       }

       return callback;
   }

   static void return_pooled_callback(zend_async_event_callback_t *callback)
   {
       // Reset callback state
       memset(callback, 0, sizeof(zend_async_event_callback_t));

       zend_mutex_lock(&pool_mutex);

       if (pool_size < MAX_POOL_SIZE) {
           callback_pool[pool_size++] = callback;
       } else {
           efree(callback);
       }

       zend_mutex_unlock(&pool_mutex);
   }

Batch Operations Pattern
========================

Grouping operations for better performance:

.. code:: c

   typedef struct {
       zend_async_event_t **events;
       int event_count;
       int completed_count;
       void (*completion_callback)(void *batch_data);
       void *batch_data;
   } batch_operation_t;

   static void batch_item_callback(zend_async_event_t *event, zend_async_event_callback_t *callback,
                                  void *result, zend_object *exception)
   {
       batch_operation_t *batch = (batch_operation_t *)callback->arg;

       __sync_fetch_and_add(&batch->completed_count, 1);

       if (batch->completed_count == batch->event_count) {
           // All operations completed
           if (batch->completion_callback) {
               batch->completion_callback(batch->batch_data);
           }
           cleanup_batch(batch);
       }
   }

**********************
 Integration Patterns
**********************

Reactor Integration Pattern
===========================

Integrating with external event loops:

.. code:: c

   typedef struct {
       my_event_loop_t *external_loop;
       HashTable *pending_events;
       bool integrated;
   } reactor_integration_t;

   static void bridge_event_callback(my_external_event_t *ext_event, void *data)
   {
       zend_async_event_t *async_event = (zend_async_event_t *)data;

       // Convert external event result to async event result
       void *result = convert_external_result(ext_event);
       zend_object *exception = check_external_error(ext_event);

       // Notify async event callbacks
       async_event->callbacks_notify(async_event, result, exception);
   }

   bool integrate_with_reactor(my_event_loop_t *loop)
   {
       if (!ZEND_ASYNC_REACTOR_IS_ENABLED()) {
           return false;
       }

       reactor_integration_t *integration = emalloc(sizeof(reactor_integration_t));
       integration->external_loop = loop;

       ALLOC_HASHTABLE(integration->pending_events);
       zend_hash_init(integration->pending_events, 32, NULL, NULL, 0);

       // Set up bridge between external loop and async events
       loop->set_callback(loop, bridge_event_callback);

       return true;
   }

******************
 Testing Patterns
******************

Mock Implementation Pattern
===========================

Creating testable async code:

.. code:: c

   #ifdef TESTING_MODE

   typedef struct {
       zend_async_event_t base;
       void *expected_result;
       zend_object *expected_exception;
       bool should_fail;
   } mock_event_t;

   static void mock_event_start(zend_async_event_t *event)
   {
       mock_event_t *mock = (mock_event_t *)event;

       // Simulate immediate completion for testing
       if (mock->should_fail) {
           event->callbacks_notify(event, NULL, mock->expected_exception);
       } else {
           event->callbacks_notify(event, mock->expected_result, NULL);
       }
   }

   mock_event_t* create_mock_event(void *result, zend_object *exception)
   {
       mock_event_t *mock = emalloc(sizeof(mock_event_t));
       memset(mock, 0, sizeof(mock_event_t));

       mock->base.start = mock_event_start;
       mock->expected_result = result;
       mock->expected_exception = exception;
       mock->should_fail = (exception != NULL);

       return mock;
   }

   #endif

*******************
 Advanced Patterns
*******************

State Machine Pattern
=====================

Managing complex async state transitions:

.. code:: c

   typedef enum {
       STATE_IDLE,
       STATE_CONNECTING,
       STATE_CONNECTED,
       STATE_SENDING,
       STATE_RECEIVING,
       STATE_ERROR,
       STATE_CLOSED
   } connection_state_t;

   typedef struct {
       connection_state_t state;
       zend_async_event_t *current_event;
       void (*state_handlers[STATE_CLOSED + 1])(void *connection);
       void *connection_data;
   } async_state_machine_t;

   static void transition_state(async_state_machine_t *sm, connection_state_t new_state)
   {
       if (sm->state != new_state) {
           sm->state = new_state;

           if (sm->state_handlers[new_state]) {
               sm->state_handlers[new_state](sm->connection_data);
           }
       }
   }

Producer-Consumer Pattern
=========================

Implementing async producer-consumer with channels:

.. code:: c

   typedef struct {
       zend_async_channel_t *channel;
       zend_coroutine_t *producer_coro;
       zend_coroutine_t *consumer_coro;
       bool shutdown_requested;
   } producer_consumer_t;

   static void producer_coroutine(void *arg)
   {
       producer_consumer_t *pc = (producer_consumer_t *)arg;

       while (!pc->shutdown_requested) {
           // Produce data
           zval data;
           produce_data(&data);

           // Send to channel (may suspend if channel is full)
           if (!pc->channel->send(pc->channel, &data)) {
               break; // Channel closed
           }
       }
   }

   static void consumer_coroutine(void *arg)
   {
       producer_consumer_t *pc = (producer_consumer_t *)arg;

       while (!pc->shutdown_requested) {
           zval data;

           // Receive from channel (may suspend if channel is empty)
           if (!pc->channel->receive(pc->channel, &data)) {
               break; // Channel closed
           }

           // Process data
           process_data(&data);
           zval_dtor(&data);
       }
   }

************************
 Best Practices Summary
************************

#. **Always check enablement** before using async APIs
#. **Provide sync fallbacks** for non-async environments
#. **Use internal context** for per-coroutine data
#. **Handle reference counting** properly
#. **Register cleanup handlers** for resource management
#. **Use appropriate error handling** patterns
#. **Consider performance** with pooling and batching
#. **Test thoroughly** with mock implementations
#. **Document async behavior** clearly for users
#. **Follow reactor integration** patterns for external loops
