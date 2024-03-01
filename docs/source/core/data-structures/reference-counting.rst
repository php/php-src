####################
 Reference counting
####################

In languages like C, when you need memory for storing data for an indefinite period of time or in a
large amount, you call ``malloc`` and ``free`` to acquire and release blocks of memory of some size.
This sounds simple on the surface but turns out to be quite tricky, mainly because the data may not
be freed for as long as it is used anywhere in the program. Sometimes this makes it unclear who is
responsible for freeing the memory, and when to do so. Failure to handle this correctly may result
in a use-after-free, double-free, or memory leak.

In PHP you usually do not need to think about memory management. The engine takes care of allocating
and freeing memory for you by tracking which values are no longer needed. It does this by assigning
a reference count to each allocated value, often abbreviated as refcount or RC. Whenever a reference
to a value is passed somewhere else, its reference count is increased to indicate the value is now
used by another party. When the party no longer needs the value, it is responsible for decreasing
the reference count. Once the reference count reaches zero, we know the value is no longer needed
anywhere, and that it may be freed.

.. code:: php

   $a = new stdClass; // RC 1
   $b = $a;           // RC 2
   unset($a);         // RC 1
   unset($b);         // RC 0, free

Reference counting is needed for types that store auxiliary data, which are the following:

-  Strings
-  Arrays
-  Objects
-  References
-  Resources

These are either reference types (objects, references and resources) or they are large types that
don't fit in a single ``zend_value`` directly (strings, arrays). Simpler types either don't store a
value at all (``null``, ``false``, ``true``) or their value is small enough to fit directly in
``zend_value`` (``int``, ``float``).

All of the reference counted types share a common initial struct sequence.

.. code:: c

   typedef struct _zend_refcounted_h {
       uint32_t refcount; /* reference counter 32-bit */
       union {
           uint32_t type_info;
       } u;
   } zend_refcounted_h;

    struct _zend_string {
        zend_refcounted_h gc;
        // ...
    };

    struct _zend_array {
        zend_refcounted_h gc;
        // ...
    };

The ``zend_refcounted_h`` struct is simple. It contains the reference count, and a ``type_info``
field that repeats some of the type information that is also stored in the ``zval``, for situations
where we're not dealing with a ``zval`` directly. It also stores some additional fields, described
under `GC flags`_.

********
 Macros
********

As with ``zval``, ``zend_refcounted_h`` members should not be accessed directly. Instead, you should
use the provided macros. There are macros that work with reference counted types directly, prefixed
with ``GC_``, or macros that work on ``zval`` values, usually prefixed with ``Z_``. Unfortunately,
naming is not always consistent.

.. list-table:: ``zval`` macros
   :header-rows: 1

   -  -  Macro
      -  Non-RC [#non-rc]_
      -  Description

   -  -  ``Z_REFCOUNT[_P]``
      -  No
      -  Returns the reference count.

   -  -  ``Z_ADDREF[_P]``
      -  No
      -  Increases the reference count.

   -  -  ``Z_TRY_ADDREF[_P]``
      -  Yes
      -  Increases the reference count. May be called on any ``zval``.

   -  -  ``zval_ptr_dtor``
      -  Yes
      -  Decreases the reference count and frees the value if the reference count reaches zero.

.. [#non-rc]

   Whether the macro works with non-reference counted types. If it does, the operation is usually a
   no-op. If it does not, using the macro on these values is undefined behavior.

.. list-table:: ``zend_refcounted_h`` macros
   :header-rows: 1

   -  -  Macro
      -  Immutable [#immutable]_
      -  Description

   -  -  ``GC_REFCOUNT[_P]``
      -  Yes
      -  Returns the reference count.

   -  -  ``GC_ADDREF[_P]``
      -  No
      -  Increases the reference count.

   -  -  ``GC_TRY_ADDREF[_P]``
      -  Yes
      -  Increases the reference count.

   -  -  ``GC_DTOR[_P]``
      -  Yes
      -  Decreases the reference count and frees the value if the reference count reaches zero.

.. [#immutable]

   Whether the macro works with immutable types, described under `Immutable reference counted types`_.

************
 Separation
************

PHP has value and reference types. Reference types are types that are shared through a reference, a
"pointer" to the value, rather than the value itself. Modifying such a value in one place changes it
for all of its observers. For example, writing to a property changes the property in every place the
object is referenced. Value types, on the other hand, are copied when passed to another party.
Modifying the original value does not affect the copy, and vice versa.

In PHP, arrays and strings are value types. Since they are also reference counted types, this
requires some special care when modifying values. In particular, we need to make sure that modifying
the value is not observable from other places. Modifying a value with RC 1 is unproblematic, since
we are the values sole owner. However, if the value has a reference count of >1, we need to create a
fresh copy before modifying it. This process is called separation or CoW (copy on write).

.. code:: php

   $a = [1, 2, 3]; // RC 1
   $b = $a;        // RC 2
   $b[] = 4;       // Separation, $a RC 1, $b RC 1
   var_dump($a);   // [1, 2, 3]
   var_dump($b);   // [1, 2, 3, 4]

***********************************
 Immutable reference counted types
***********************************

Sometimes, even a reference counted type is not reference counted. When PHP runs in a multi-process
or multi-threaded environment with opcache enabled, it shares some common values between processes
or threads to reduce memory consumption. As you may know, sharing memory between processes or
threads can be tricky and requires special care when modifying values. In particular, modification
usually requires exclusive access to the memory so that the other processes or threads wait until
the value is done being updated. In this case, this synchronization is avoided by making the value
immutable and never modifying the reference count. Such values will receive the ``GC_IMMUTABLE``
flag in their ``gc->u.type_info`` field.

Some macros like ``GC_TRY_ADDREF`` will guard against immutable values. You should not use immutable
values on some macros, like ``GC_ADDREF``. This will result in undefined behavior, because the macro
will not check whether the value is immutable before performing the reference count modifications.
You may execute PHP with the ``-d opcache.protect_memory=1`` flag to mark the shared memory as
read-only and trigger a hardware exception if the code accidentally attempts to modify it.

*****************
 Cycle collector
*****************

Sometimes, reference counting is not enough. Consider the following example:

.. code:: php

   $a = new stdClass;
   $b = new stdClass;
   $a->b = $b;
   $b->a = $a;
   unset($a);
   unset($b);

When this code finishes, the reference count of both instances of ``stdClass`` will still be 1, as
they reference each other. This is called a reference cycle.

PHP implements a cycle collector that detects such cycles and frees values that are only reachable
through their own references. The cycle collector will record values that may be involved in a
cycle, and run when this buffer becomes full. It is also possible to invoke it explicitly by calling
the ``gc_collect_cycles()`` function. The cycle collectors design is described in the `Cycle
collector <todo>`_ chapter.

**********
 GC flags
**********

.. code:: c

   /* zval_gc_flags(zval.value->gc.u.type_info) (common flags) */
   #define GC_NOT_COLLECTABLE  (1<<4)
   #define GC_PROTECTED        (1<<5) /* used for recursion detection */
   #define GC_IMMUTABLE        (1<<6) /* can't be changed in place */
   #define GC_PERSISTENT       (1<<7) /* allocated using malloc */
   #define GC_PERSISTENT_LOCAL (1<<8) /* persistent, but thread-local */

The ``GC_NOT_COLLECTABLE`` flag indicates that the value may not be involved in a reference cycle.
This allows for a fast way to detect values that don't need to be added to the cycle collector
buffer. Only arrays and objects may actually be involved in reference cycles.

The ``GC_PROTECTED`` flag is used to protect against recursion in various internal functions. For
example, ``var_dump`` recursively prints the contents of values, and marks visited values with the
``GC_PROTECTED`` flag. If the value is recursive, it prevents the same value from being visited
again.

``GC_IMMUTABLE`` has been discussed in `Immutable reference counted types`_.

The ``GC_PERSISTENT`` flag indicates that the value was allocated using ``malloc``, instead of PHPs
own allocator. Usually, such values are alive for the entire lifetime of the process, instead of
being freed at the end of the request. See the `Zend allocator <todo>`_ chapter for more
information.

The ``GC_PERSISTENT_LOCAL`` flag indicates that a ``CG_PERSISTENT`` value is only accessibly in one
thread, and is thus still safe to modify. This flag is only used in debug builds to satisfy an
``assert``.
