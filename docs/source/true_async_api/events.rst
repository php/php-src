################
 Events
################

The TrueAsync API revolves around the concept of **events**. An event represents any asynchronous action that can produce a result or error in the future. All specific event types share a common base structure ``zend_async_event_t`` which provides memory management, state flags and a standard lifecycle.

**********************
Creating an Event
**********************

To create an event you typically call one of the ``ZEND_ASYNC_NEW_*`` macros which delegate to the underlying implementation. Each macro returns a structure that contains ``zend_async_event_t`` as its first member.

.. code:: c

   zend_async_timer_event_t *timer = ZEND_ASYNC_NEW_TIMER_EVENT(1000, false);
   timer->base.start(&timer->base);

The ``base`` field exposes the common functions ``start``, ``stop`` and ``dispose`` as well as reference counting helpers.

*****************
Event Flags
*****************

Each event has a ``flags`` field that controls its state and behaviour. The most
important flags are:

``ZEND_ASYNC_EVENT_F_CLOSED``
    The event has been closed and may no longer be started or stopped.

``ZEND_ASYNC_EVENT_F_RESULT_USED``
    Indicates that the result will be consumed by an awaiting context. When this
    flag is set the awaiting code keeps the result alive until it is processed
    in the exception handler.

``ZEND_ASYNC_EVENT_F_EXC_CAUGHT``
    Marks that an error produced by the event was caught in a callback and will
    not be rethrown automatically.

``ZEND_ASYNC_EVENT_F_ZVAL_RESULT``
    Signals that the callback result is a ``zval`` pointer. The TrueAsync core
    will properly increment the reference count before inserting the value into
    userland arrays.

``ZEND_ASYNC_EVENT_F_ZEND_OBJ``
    Specifies that the structure also acts as a Zend object implementing
    ``Awaitable``.

``ZEND_ASYNC_EVENT_F_NO_FREE_MEMORY``
    The dispose handler must not free the memory of the event itself. This is
    used when the event is embedded into another structure.

``ZEND_ASYNC_EVENT_F_EXCEPTION_HANDLED``
    Set by the callback once it has fully processed an exception. If this flag
    is not set the exception will be rethrown after callbacks finish.

``ZEND_ASYNC_EVENT_F_REFERENCE``
    Indicates that this structure is only an event reference which stores a
    pointer to the real event.

``ZEND_ASYNC_EVENT_F_OBJ_REF``
    Marks that the event keeps a pointer to the Zend object in its
    ``extra_offset`` region so reference counting works through the object.

Convenience macros such as ``ZEND_ASYNC_EVENT_SET_CLOSED`` and
``ZEND_ASYNC_EVENT_IS_EXCEPTION_HANDLED`` are provided to manipulate these
flags.

******************
Event Callbacks
******************

The core maintains a dynamic vector of callbacks for each event. Implementations
provide the ``add_callback`` and ``del_callback`` methods which internally use
``zend_async_callbacks_push`` and ``zend_async_callbacks_remove``. The
``zend_async_callbacks_notify`` helper iterates over all registered callbacks
and passes the result or exception.  If a ``notify_handler`` is set on the event
it is invoked first and can adjust the result or exception as needed.  The
handler is expected to call ``ZEND_ASYNC_CALLBACKS_NOTIFY_FROM_HANDLER`` to
forward the values to all listeners.  The ``php-async`` extension provides
convenience macros ``ZEND_ASYNC_CALLBACKS_NOTIFY`` for regular use,
``ZEND_ASYNC_CALLBACKS_NOTIFY_FROM_HANDLER`` when called from inside a
``notify_handler``, and ``ZEND_ASYNC_CALLBACKS_NOTIFY_AND_CLOSE`` to close the
event before notifying listeners.

The following example shows a libuv poll event that dispatches its callbacks
once the underlying handle becomes readable:

.. code:: c

   static void on_poll_event(uv_poll_t *handle, int status, int events) {
       async_poll_event_t *poll = handle->data;
       zend_object *exception = NULL;

       if (status < 0) {
           exception = async_new_exception(
               async_ce_input_output_exception,
               "Input output error: %s",
               uv_strerror(status)
           );
       }

       poll->event.triggered_events = events;

       ZEND_ASYNC_CALLBACKS_NOTIFY(&poll->event.base, NULL, exception);

       if (exception != NULL) {
           zend_object_release(exception);
       }
   }

*************************
Event Coroutines
*************************

Coroutines themselves are implemented as events using the ``zend_coroutine_t`` structure. When a coroutine yields, its waker waits on multiple events and resumes the coroutine once any of them triggers.

.. code:: c

   // Spawn a coroutine that waits for a timer
   zend_coroutine_t *co = ZEND_ASYNC_SPAWN(NULL);
   zend_async_resume_when(co, &timer->base, false, zend_async_waker_callback_resolve, NULL);
   zend_async_enqueue_coroutine(co);

When the coroutine finishes execution the event triggers again to deliver the
result or exception. The coroutine implementation marks the callback result as a
``zval`` value using ``ZEND_ASYNC_EVENT_SET_ZVAL_RESULT``. Callback handlers may
also set ``ZEND_ASYNC_EVENT_SET_EXCEPTION_HANDLED`` to indicate that the thrown
exception has been processed and should not be rethrown by the runtime.

****************************
Extending Events
****************************

Custom event types embed ``zend_async_event_t`` at the beginning of their
structure and may allocate additional memory beyond the end of the struct.
The ``extra_size`` argument in ``ZEND_ASYNC_NEW_*_EX`` controls how much extra
space is reserved, and ``extra_offset`` records where that region begins.

.. code:: c

   // Allocate extra space for user data
   zend_async_poll_event_t *poll = ZEND_ASYNC_NEW_POLL_EVENT_EX(fd, false, sizeof(my_data_t));
   my_data_t *data = (my_data_t *)((char*)poll + poll->base.extra_offset);

The libuv backend defines event wrappers that embed libuv handles. A timer
event, for example, extends ``zend_async_timer_event_t`` as follows:

.. code:: c

   typedef struct {
       zend_async_timer_event_t event;
       uv_timer_t uv_handle;
   } async_timer_event_t;

   // Initialize callbacks for the event
   event->event.base.add_callback = libuv_add_callback;
   event->event.base.del_callback = libuv_remove_callback;
   event->event.base.start = libuv_timer_start;
   event->event.base.stop = libuv_timer_stop;
   event->event.base.dispose = libuv_timer_dispose;

Every extended event defines its own ``start``, ``stop`` and ``dispose``
functions.  The dispose handler must release all resources associated with
the event and is called when the reference count reaches ``1``.  It is
common to stop the event first and then close the underlying libuv handle so
that memory gets freed in the ``uv_close`` callback.

.. code:: c

   static void libuv_timer_dispose(zend_async_event_t *event)
   {
       if (ZEND_ASYNC_EVENT_REF(event) > 1) {
           ZEND_ASYNC_EVENT_DEL_REF(event);
           return;
       }

       if (event->loop_ref_count > 0) {
           event->loop_ref_count = 1;
           event->stop(event);
       }

       zend_async_callbacks_free(event);

       async_timer_event_t *timer = (async_timer_event_t *)event;
       uv_close((uv_handle_t *)&timer->uv_handle, libuv_close_handle_cb);
   }

If ``ZEND_ASYNC_EVENT_F_NO_FREE_MEMORY`` is set the dispose handler must not
free the event memory itself because the structure is embedded in another
object (e.g. ``async_coroutine_t``).  The libuv close callback will only free
the libuv handle in this case.

***********************
Custom Event Callbacks
***********************

Callbacks can also be extended to store additional state.  The await logic in
``php-async`` defines a callback that inherits from
``zend_coroutine_event_callback_t`` and keeps a reference to the awaiting
context:

.. code:: c

   typedef struct {
       zend_coroutine_event_callback_t callback;
       async_await_context_t *await_context;
       zval key;
       zend_async_event_callback_dispose_fn prev_dispose;
   } async_await_callback_t;

   async_await_callback_t *cb = ecalloc(1, sizeof(async_await_callback_t));
   cb->callback.base.callback = async_waiting_callback;
   cb->await_context = ctx;
   zend_async_resume_when(co, awaitable, false, NULL, &cb->callback);

***********************
Events as Zend Objects
***********************

If ``ZEND_ASYNC_EVENT_F_ZEND_OBJ`` is set, the event also acts as a Zend object implementing ``Awaitable``. The ``zend_object_offset`` field stores the location of the ``zend_object`` within the structure. Reference counting macros automatically use either the internal counter or ``GC_REFCOUNT`` depending on this flag.

This allows events to be exposed to userland seamlessly while keeping the internal lifecycle consistent.

For events that are destroyed asynchronously (e.g. libuv timers) the actual
event structure cannot be a Zend object.  Instead a lightweight reference
structure is used.  ``ZEND_ASYNC_EVENT_REF_PROLOG`` reserves the required fields
in the Zend object and ``ZEND_ASYNC_EVENT_REF_SET`` stores the pointer to the
real event together with the ``zend_object`` offset.  The event must then be
flagged with ``ZEND_ASYNC_EVENT_WITH_OBJECT_REF`` so that reference counting
delegates to the object.

When accessing the event from userland objects use
``ZEND_ASYNC_OBJECT_TO_EVENT`` and ``ZEND_ASYNC_EVENT_TO_OBJECT`` which handle
both direct and reference-based layouts transparently.

The ``php-async`` extension provides ``Async\\Timeout`` objects that embed a
timer event.  Recent updates introduce a helper API for linking an event with a
Zend object.  A helper macro ``ZEND_ASYNC_EVENT_REF_PROLOG`` reserves fields at
the beginning of the object to hold an event reference.  ``ZEND_ASYNC_EVENT_REF_SET``
stores the pointer to the newly created event and the offset of the ``zend_object``
inside the structure.  ``ZEND_ASYNC_EVENT_WITH_OBJECT_REF`` then marks the event
so reference counting will use the Zend object rather than the internal counter.

The object factory now uses these helpers when creating the timer::

   static zend_object *async_timeout_create(const zend_ulong ms, const bool is_periodic)
   {
       async_timeout_object_t *object =
           ecalloc(1, sizeof(async_timeout_object_t) +
                   zend_object_properties_size(async_ce_timeout));

       zend_async_event_t *event = (zend_async_event_t *) ZEND_ASYNC_NEW_TIMER_EVENT_EX(
           ms, is_periodic, sizeof(async_timeout_ext_t)
       );

       if (UNEXPECTED(event == NULL)) {
           efree(object);
           return NULL;
       }

       ZEND_ASYNC_EVENT_REF_SET(object, XtOffsetOf(async_timeout_object_t, std), (zend_async_timer_event_t *) event);
       ZEND_ASYNC_EVENT_WITH_OBJECT_REF(event);

       async_timeout_ext_t *timeout = ASYNC_TIMEOUT_FROM_EVENT(event);
       timeout->std = &object->std;
       timeout->prev_dispose = event->dispose;
       event->notify_handler = timeout_before_notify_handler;
       event->dispose = async_timeout_event_dispose;

       zend_object_std_init(&object->std, async_ce_timeout);
       object_properties_init(&object->std, async_ce_timeout);
       object->std.handlers = &async_timeout_handlers;

       return &object->std;
   }

   static void timeout_before_notify_handler(zend_async_event_t *event,
                                             void *result,
                                             zend_object *exception)
   {
       if (UNEXPECTED(exception != NULL)) {
           ZEND_ASYNC_CALLBACKS_NOTIFY_FROM_HANDLER(event, result, exception);
           return;
       }

       zend_object *timeout_exception = async_new_exception(
           async_ce_timeout_exception,
           "Timeout occurred after %lu milliseconds",
           ((zend_async_timer_event_t *) event)->timeout
       );

       ZEND_ASYNC_CALLBACKS_NOTIFY_FROM_HANDLER(event, result, timeout_exception);
       zend_object_release(timeout_exception);
   }

.. note::

   Events must not be exposed as Zend objects if their memory is released
   asynchronously.  Zend assumes that object destruction happens entirely
   during the ``zend_object_release`` call and cannot wait for callbacks such as
   ``uv_close`` to free the underlying event.  The ``Async\\Timeout`` class will
   be redesigned to avoid this pattern.

