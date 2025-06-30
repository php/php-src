################
 Coroutines
################

Coroutines are the core building blocks of the TrueAsync API. They represent lightweight execution contexts that can be suspended and resumed, enabling cooperative multitasking without traditional thread overhead. Every coroutine is implemented as an event (``zend_coroutine_t`` extends ``zend_async_event_t``) and participates in the standard event lifecycle.

*******************
 Coroutine Basics
*******************

A coroutine represents a function or code block that can be paused during execution and resumed later. Unlike traditional threads, coroutines are cooperatively scheduled - they yield control voluntarily rather than being preemptively interrupted.

Key characteristics:

* **Lightweight** - Minimal memory overhead compared to threads
* **Cooperative** - Explicit yield points, no race conditions
* **Event-based** - Coroutines are events that can await other events
* **Scoped** - Each coroutine runs within a scope for context isolation

***********************
 Creating Coroutines
***********************

The TrueAsync API provides several ways to create coroutines:

.. code:: c

   // Create a new coroutine within default scope
   zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
   
   // Create with specific scope
   zend_async_scope_t *scope = ZEND_ASYNC_NEW_SCOPE(parent_scope);
   zend_coroutine_t *coro = ZEND_ASYNC_SPAWN_WITH(scope);
   
   // Create with scope provider object
   zend_coroutine_t *coro = ZEND_ASYNC_SPAWN_WITH_PROVIDER(scope_provider);

Coroutines can be either:

**Userland coroutines**
   Execute PHP code through ``zend_fcall_t`` callback mechanism

**Internal coroutines**  
   Execute C code through ``zend_coroutine_entry_t`` function pointer

*************************
 Coroutine Lifecycle
*************************

Coroutines follow a well-defined lifecycle:

1. **Creation** - Coroutine is spawned but not yet started
2. **Enqueue** - Added to execution queue via ``ZEND_ASYNC_ENQUEUE_COROUTINE()``
3. **Execution** - Runs until suspension point or completion
4. **Suspension** - Yields control while waiting for events
5. **Resume** - Continues execution when events complete
6. **Completion** - Finishes with result or exception

.. code:: c

   zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
   
   // Set up coroutine execution
   coro->fcall = &my_function_call;
   coro->scope = current_scope;
   
   // Start execution
   ZEND_ASYNC_ENQUEUE_COROUTINE(coro);
   
   // Later: resume with result
   ZEND_ASYNC_RESUME(coro);
   
   // Or resume with error
   ZEND_ASYNC_RESUME_WITH_ERROR(coro, exception, transfer_error);

*************************
 Coroutine Context
*************************

Each coroutine maintains its own context information:

**Scope Context** (``zend_async_scope_t *scope``)
   Links to parent scope for hierarchical context management

**Async Context** (``zend_async_context_t *context``) 
   User-defined context variables accessible from PHP code

**Internal Context** (``HashTable *internal_context``)
   Extension-private data storage with numeric keys

**Switch Handlers** (``zend_coroutine_switch_handlers_vector_t *switch_handlers``)
   Functions called when entering/leaving the coroutine

****************************
 Context Switch Handlers
****************************

Context switch handlers are one of the most powerful features of the coroutine system. They allow extensions to hook into coroutine context switches and perform necessary cleanup, initialization, or state management.

Handler Function Signature
===========================

.. code:: c

   typedef bool (*zend_coroutine_switch_handler_fn)(
       zend_coroutine_t *coroutine,
       bool is_enter,     /* true = entering coroutine, false = leaving */
       bool is_finishing  /* true = coroutine is finishing */
       /* returns: true = keep handler, false = remove handler after execution */
   );

The handler receives three parameters:

* **coroutine** - The coroutine being switched to/from
* **is_enter** - ``true`` when entering coroutine, ``false`` when leaving
* **is_finishing** - ``true`` when coroutine is finishing execution

The return value determines handler lifetime:

* **true** - Keep handler for future context switches
* **false** - Remove handler immediately after execution

This allows handlers to perform one-time initialization and then remove themselves automatically.

Registering Handlers
====================

There are two types of switch handlers:

**Per-Coroutine Handlers**

.. code:: c

   // Add handler to specific coroutine
   uint32_t handler_id = ZEND_COROUTINE_ADD_SWITCH_HANDLER(coroutine, my_handler);
   
   // Remove specific handler
   zend_coroutine_remove_switch_handler(coroutine, handler_id);

**Global Main Coroutine Handlers**

.. code:: c

   // Register global handler for all main coroutines
   ZEND_ASYNC_ADD_MAIN_COROUTINE_START_HANDLER(my_main_handler);

Global handlers are automatically copied to each main coroutine when it starts and can remove themselves by returning ``false``.

Practical Example: Output Buffering
====================================

The ``main/output.c`` module provides an excellent example of context switch handlers in action. The output system needs to isolate output buffers between the main execution context and coroutines to prevent buffer conflicts.

.. code:: c

   static bool php_output_main_coroutine_start_handler(
       zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (false == is_enter || OG(handlers).elements == 0) {
           return false;  /* Remove handler - no work to do */
       }

       /* Create coroutine-specific output context */
       php_output_context_t *ctx = ecalloc(1, sizeof(php_output_context_t));
       php_output_init_async_context(ctx);

       /* Copy output handlers from global to coroutine context */
       php_output_handler **src_handlers = (php_output_handler **)zend_stack_base(&OG(handlers));
       int handler_count = zend_stack_count(&OG(handlers));

       for (int i = 0; i < handler_count; i++) {
           php_output_handler *handler = src_handlers[i];
           zend_stack_push(&ctx->handlers, &handler);
       }

       /* Store context in coroutine internal_context */
       zval ctx_zval;
       ZVAL_PTR(&ctx_zval, ctx);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, php_output_context_key, &ctx_zval);

       /* Clean global handlers to avoid conflicts */
       zend_stack_destroy(&OG(handlers));
       zend_stack_init(&OG(handlers), sizeof(php_output_handler *));
       OG(active) = NULL;

       /* Add cleanup callback for coroutine completion */
       zend_coroutine_event_callback_t *cleanup_callback =
           zend_async_coroutine_callback_new(coroutine, php_output_coroutine_cleanup_callback, 0);
       coroutine->event.add_callback(&coroutine->event, &cleanup_callback->base);
       
       return false;  /* Remove handler - initialization is complete */
   }

This handler:

1. **Detects main coroutine entry** - Only acts when ``is_enter`` is true
2. **Creates isolated context** - Copies output handlers to coroutine-specific storage
3. **Stores in internal_context** - Uses numeric key for fast access
4. **Cleans global state** - Prevents conflicts between main and coroutine contexts
5. **Registers cleanup** - Ensures proper resource cleanup on coroutine completion
6. **Removes itself** - Returns ``false`` since initialization is one-time only

When to Use Context Switch Handlers
====================================

Context switch handlers are ideal for:

**Resource Isolation**
   Separating global state between main execution and coroutines (like output buffers, error handlers, etc.)

**Context Migration**
   Moving data from one execution context to another

**State Initialization**
   Setting up coroutine-specific resources or configurations

**Cleanup Management**
   Registering cleanup callbacks for coroutine completion

**Performance Optimization**
   Pre-computing or caching data when entering frequently-used coroutines

**************************
 Internal Context API
**************************

The internal context system provides a type-safe, efficient way for PHP extensions to store private data associated with coroutines. Unlike userland context variables, internal context uses numeric keys for faster access and is completely isolated from PHP code.

Why Internal Context?
======================

Before internal context, extensions had limited options for storing coroutine-specific data:

* **Global variables** - Not coroutine-safe, cause conflicts
* **Object properties** - Not available for internal coroutines
* **Manual management** - Complex, error-prone cleanup

Internal context solves these problems by providing:

* **Automatic cleanup** - Data is freed when coroutine completes
* **Type safety** - Values stored as ``zval`` with proper reference counting
* **Uniqueness** - Each extension gets private numeric keys
* **Performance** - Hash table lookup by integer key

Key Allocation
==============

Extensions allocate unique keys during module initialization:

.. code:: c

   static uint32_t my_extension_context_key = 0;

   PHP_MINIT_FUNCTION(my_extension)
   {
       // Allocate unique key for this extension
       my_extension_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("my_extension_data");
       return SUCCESS;
   }

The ``ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC`` macro:

* Takes a static string identifier (for debugging)
* Returns a unique numeric key
* Validates that the same string address always gets the same key
* Is thread-safe in ZTS builds

Storing and Retrieving Data
===========================

.. code:: c

   void my_coroutine_handler(zend_coroutine_t *coroutine)
   {
       // Store data in coroutine context
       zval my_data;
       ZVAL_LONG(&my_data, 42);
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, my_extension_context_key, &my_data);
       
       // Later: retrieve data
       zval *stored_data = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, my_extension_context_key);
       if (stored_data && Z_TYPE_P(stored_data) == IS_LONG) {
           long value = Z_LVAL_P(stored_data);
           // Use value...
       }
       
       // Optional: remove data explicitly
       ZEND_ASYNC_INTERNAL_CONTEXT_UNSET(coroutine, my_extension_context_key);
   }

The API automatically handles:

* **Reference counting** - ``zval`` reference counts are managed properly
* **Memory management** - All context data is freed when coroutine ends
* **Thread safety** - Operations are safe in ZTS builds

Advanced Usage Patterns
========================

**Caching expensive computations:**

.. code:: c

   zval *cached_result = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, cache_key);
   if (!cached_result) {
       // Expensive operation
       zval computed_value;
       ZVAL_STRING(&computed_value, expensive_computation());
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, cache_key, &computed_value);
       cached_result = &computed_value;
   }

**Tracking state across suspensions:**

.. code:: c

   // Before suspending
   zval state;
   ZVAL_LONG(&state, OPERATION_IN_PROGRESS);
   ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, state_key, &state);
   
   // After resuming
   zval *state_val = ZEND_ASYNC_INTERNAL_CONTEXT_FIND(coroutine, state_key);
   if (state_val && Z_LVAL_P(state_val) == OPERATION_IN_PROGRESS) {
       // Continue operation...
   }

**Storing complex objects:**

.. code:: c

   zval object_container;
   ZVAL_OBJ(&object_container, my_object);
   ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, object_key, &object_container);
   // Object reference count is automatically incremented

Best Practices
==============

1. **Allocate keys in MINIT** - Ensures keys are available when needed
2. **Use descriptive key names** - Helps with debugging and maintenance  
3. **Check return values** - ``FIND`` returns ``NULL`` if key doesn't exist
4. **Validate data types** - Always check ``Z_TYPE_P`` before accessing data
5. **Clean up appropriately** - Use ``UNSET`` for early cleanup if needed

****************
 Coroutine API
****************

Core Functions
==============

.. code:: c

   // Coroutine creation and management
   zend_coroutine_t *ZEND_ASYNC_SPAWN();
   zend_coroutine_t *ZEND_ASYNC_NEW_COROUTINE(scope);
   void ZEND_ASYNC_ENQUEUE_COROUTINE(coroutine);
   
   // Execution control
   void ZEND_ASYNC_SUSPEND();
   void ZEND_ASYNC_RESUME(coroutine);
   void ZEND_ASYNC_RESUME_WITH_ERROR(coroutine, error, transfer_error);
   void ZEND_ASYNC_CANCEL(coroutine, error, transfer_error);
   
   // Context switch handlers
   uint32_t ZEND_COROUTINE_ADD_SWITCH_HANDLER(coroutine, handler);
   bool zend_coroutine_remove_switch_handler(coroutine, handler_index);
   void ZEND_ASYNC_ADD_MAIN_COROUTINE_START_HANDLER(handler);

State Checking Macros
=====================

.. code:: c

   // Coroutine state
   ZEND_COROUTINE_IS_STARTED(coroutine)
   ZEND_COROUTINE_IS_FINISHED(coroutine) 
   ZEND_COROUTINE_IS_CANCELLED(coroutine)
   ZEND_COROUTINE_SUSPENDED(coroutine)
   ZEND_COROUTINE_IS_MAIN(coroutine)
   
   // Current context
   ZEND_ASYNC_CURRENT_COROUTINE
   ZEND_ASYNC_CURRENT_SCOPE

Memory Management
=================

Coroutines follow standard event reference counting:

.. code:: c

   ZEND_ASYNC_EVENT_ADD_REF(&coroutine->event);   // Increment reference
   ZEND_ASYNC_EVENT_RELEASE(&coroutine->event);   // Decrement and cleanup if needed

The coroutine automatically cleans up:

* Internal context data
* Switch handlers
* Event callbacks
* Scope references

********************
 Best Practices
********************

1. **Always check if async is enabled** before using coroutine APIs
2. **Use appropriate scopes** to maintain context isolation
3. **Handle exceptions properly** in coroutine callbacks
4. **Register cleanup handlers** for long-running operations
5. **Use internal context** instead of global variables for coroutine data
6. **Remove one-time handlers** by returning ``false`` from switch handlers
7. **Validate coroutine state** before performing operations

Example: Complete Coroutine Usage
==================================

.. code:: c

   static uint32_t my_context_key = 0;

   PHP_MINIT_FUNCTION(my_extension)
   {
       my_context_key = ZEND_ASYNC_INTERNAL_CONTEXT_KEY_ALLOC("my_extension");
       ZEND_ASYNC_ADD_MAIN_COROUTINE_START_HANDLER(my_main_handler);
       return SUCCESS;
   }

   static bool my_main_handler(zend_coroutine_t *coroutine, bool is_enter, bool is_finishing)
   {
       if (!is_enter) return true;
       
       // Initialize coroutine-specific data
       zval init_data;
       ZVAL_STRING(&init_data, "initialized");
       ZEND_ASYNC_INTERNAL_CONTEXT_SET(coroutine, my_context_key, &init_data);
       
       return false; // Remove handler after initialization
   }

   void my_async_operation(void)
   {
       if (!ZEND_ASYNC_IS_ENABLED()) {
           return; // Fallback to synchronous operation
       }
       
       zend_coroutine_t *coro = ZEND_ASYNC_SPAWN();
       coro->internal_entry = my_coroutine_function;
       
       ZEND_ASYNC_ENQUEUE_COROUTINE(coro);
       ZEND_ASYNC_SUSPEND(); // Wait for completion
   }

This comprehensive coroutine system enables powerful asynchronous programming patterns while maintaining clean separation of concerns and proper resource management.