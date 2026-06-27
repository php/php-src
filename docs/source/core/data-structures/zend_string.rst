#############
 zend_string
#############

In C, strings are represented as sequential lists of characters, ``char*`` or ``char[]``. The end of
the string is usually indicated by the special NUL character, ``'\0'``. This comes with a few
significant downsides:

-  Calculating the length of the string is expensive, as it requires walking the entire string to
   look for the terminating NUL character.
-  The string may not contain the NUL character itself.
-  It is easy to run into buffer overflows if the NUL byte is accidentally missing.

php-src uses the ``zend_string`` struct as an abstraction over ``char*``, which explicitly stores
the strings length, along with some other fields. It looks as follows:

.. code:: c

   struct _zend_string {
       zend_refcounted_h gc;
       zend_ulong        h; /* hash value */
       size_t            len;
       char              val[1];
   };

The ``gc`` field is used for :doc:`./reference-counting`. The ``h`` field contains a hash value,
which is used for `hash table <todo>`__ lookups. The ``len`` field stores the length of the string
in bytes, and the ``val`` field contains the actual string data.

You may wonder why the ``val`` field is declared as ``char val[1]``. This is called the `struct
hack`_ in C. It is used to create structs with a flexible size, namely by allowing the last element
to be expanded arbitrarily. In this case, the size of ``zend_string`` depends on the string's length,
which is determined at runtime (see ``_ZSTR_STRUCT_SIZE``). When allocating the string, we append
enough bytes to the allocation to hold the strings content.

.. _struct hack: https://www.geeksforgeeks.org/struct-hack/

Here's a basic example of how to use ``zend_string``:

.. code:: c

   // Allocate the string.
   zend_string *string = ZSTR_INIT_LITERAL("Hello world!", /* persistent */ false);
   // Write it to the output buffer.
   zend_write(ZSTR_VAL(string), ZSTR_LEN(string));
   // Decrease the reference count and free it if necessary.
   zend_string_release(string);

``ZSTR_INIT_LITERAL`` creates a ``zend_string`` from a string literal. It is just a wrapper around
``zend_string_init(char *string, size_t length, bool persistent)`` that provides the length of the
string at compile time. The ``persistent`` parameter indicates whether the string is allocated using
``malloc`` (``persistent == true``) or ``emalloc``, `PHPs custom allocator <todo>`__ (``persistent
== false``) that is emptied after each request.

When you're done using the string, you must call ``zend_string_release``, or the memory will leak.
``zend_string_release`` will automatically call ``malloc`` or ``emalloc``, depending on how the
string was allocated. After releasing the string, you must not access any of its fields anymore, as
it may have been freed if you were its last user.

*****
 API
*****

The string API is defined in ``Zend/zend_string.h``. It contains creation, resizing, comparison,
hashing, and interning helpers.

.. list-table:: Creation and allocation APIs
   :header-rows: 1

   -  -  Function/Macro [#persistent]_
      -  Description

   -  -  ``ZSTR_INIT_LITERAL(s, p)``
      -  Creates a new string from a C string literal. This is a convenience wrapper around
         ``zend_string_init`` that computes the literal length at compile time.

   -  -  ``zend_string_init(s, l, p)``
      -  Creates a new string from a character buffer.

   -  -  ``zend_string_init_fast(s, l)``
      -  Fast-path initializer that may return shared immutable strings for lengths 0 and 1.

   -  -  ``zend_string_alloc(l, p)``
      -  Allocates a new string of length ``l`` without initializing its contents.

   -  -  ``zend_string_safe_alloc(n, m, l, p)``
      -  Allocates ``n * m + l`` bytes of payload with overflow checks.

   -  -  ``zend_string_concat2(s1, l1, s2, l2)``
      -  Creates a non-persistent string by concatenating two character buffers.

   -  -  ``zend_string_concat3(...)``
      -  Same as ``zend_string_concat2``, but for three character buffers.

   -  -  ``ZSTR_EMPTY_ALLOC()``
      -  Returns an immutable, empty string. This does not allocate memory.

   -  -  ``ZSTR_CHAR(c)``
      -  Returns an immutable, single-character string. This does not allocate memory.

   -  -  ``ZSTR_KNOWN(ZEND_STR_const)``

      -  Gets an immutable, predefined string. Used for string common within PHP itself, e.g.
         ``"class"``. See ``ZEND_KNOWN_STRINGS`` in ``Zend/zend_string.h``. This does not allocate
         memory.

   -  -  ``ZSTR_MAX_OVERHEAD``
      -  Maximum allocator/header overhead used by ``zend_string``.

   -  -  ``ZSTR_MAX_LEN``
      -  Maximum representable payload length for a ``zend_string``.

.. list-table:: Resizing and copy-on-write APIs
   :header-rows: 1

   -  -  Function/Macro [#persistent]_
      -  Description

   -  -  ``zend_string_realloc(s, l, p)``

      -  Changes the size of the string. If the string has a reference count greater than 1 or if
         the string is interned, a new string is created. You must always use the return value of
         this function, as the original array may have been moved to a new location in memory.

   -  -  ``zend_string_safe_realloc(s, n, m, l, p)``
      -  Resizes a string to ``n * m + l`` bytes with overflow checks. Allocates a string of length
         ``n * m + l``. This function is commonly useful for encoding changes.

   -  -  ``zend_string_extend(s, l, p)``
      -  Extends a string to a larger length (``l >= ZSTR_LEN(s)``).

   -  -  ``zend_string_truncate(s, l, p)``
      -  Shrinks a string to a smaller length (``l <= ZSTR_LEN(s)``).

   -  -  ``zend_string_dup(s, p)``
      -  Creates a real copy in a new allocation, except for interned strings (which are returned
         unchanged).

   -  -  ``zend_string_separate(s, p)``
      -  Ensures unique ownership (copy-on-write): duplicates if shared or interned, otherwise
         reuses ``s`` after resetting cached metadata.

.. [#persistent]

   ``s`` = ``zend_string``, ``l`` = ``length``, ``p`` = ``persistent``, ``n * m + l`` = checked size
   expression used for safe allocation/reallocation.

As per php-src fashion, you are not supposed to access the ``zend_string`` fields directly. Instead,
use the following macros. There are macros for both ``zend_string`` and ``zvals`` known to contain
strings.

.. list-table:: Accessor macros
   :header-rows: 1

   -  -  ``zend_string``
      -  ``zval``
      -  Description

   -  -  ``ZSTR_LEN``
      -  ``Z_STRLEN[_P]``
      -  Returns the length of the string in bytes.

   -  -  ``ZSTR_VAL``
      -  ``Z_STRVAL[_P]``
      -  Returns the string data as a ``char*``.

   -  -  ``ZSTR_HASH``
      -  ``Z_STRHASH[_P]``
      -  Computes the string hash if it hasn't already been, and returns it.

   -  -  ``ZSTR_H``
      -  \-
      -  Returns the string hash. This macro assumes that the hash has already been computed.

.. list-table:: String property macros
   :header-rows: 1

   -  -  Macro
      -  Description
   -  -  ``ZSTR_IS_INTERNED(s)``
      -  Checks whether a string is interned.
   -  -  ``ZSTR_IS_VALID_UTF8(s)``
      -  Checks whether a string has the ``IS_STR_VALID_UTF8`` flag set.

.. list-table:: Reference counting and lifetime APIs
   :header-rows: 1

   -  -  Function/Macro [#persistent]_
      -  Description

   -  -  ``zend_string_copy(s)``
      -  Increases the reference count and returns the same string. The reference count is not
         increased if the string is interned.

   -  -  ``zend_string_refcount(s)``
      -  Returns the reference count. Interned strings always report ``1``.

   -  -  ``zend_string_addref(s)``

      -  Increments the reference count of a non-interned string. the function that is used most
         often by far is zend_string_copy(). This function not only increments the refcount, but
         also returns the original string. This makes code more readable in practice.

   -  -  ``zend_string_delref(s)``
      -  Decrements the reference count of a non-interned string.

   -  -  ``zend_string_release(s)``
      -  Decreases the reference count and frees the string if it goes to 0.

   -  -  ``zend_string_release_ex(s, p)``
      -  Like ``zend_string_release()``, but allows you to specify whether the passed string is
         persistent or non-persistent. If it is persistent, ``p`` should be ``0``.

   -  -  ``zend_string_free(s)``

      -  Frees a non-interned string directly. The caller must ensure it is no longer shared.
         Requires refcount 1 or immutable.You should avoid using these functions, as it is easy to
         introduce critical bugs when some API changes from returning new strings to reusing
         existing ones.

   -  -  ``zend_string_efree(s)``
      -  Similar to ``zend_string_free``. Frees a non-persistent, non-interned string with
         ``efree``. Requires refcount 1 and not immutable.

There are various functions to compare strings.

.. list-table:: Comparison APIs
   :header-rows: 1

   -  -  Function/Macro
      -  Description
   -  -  ``zend_string_equals(s1, s2)``
      -  Full equality check for two ``zend_string`` values.
   -  -  ``zend_string_equal_content(s1, s2)``
      -  Full equality check assuming both arguments are ``zend_string`` pointers.
   -  -  ``zend_string_equal_val(s1, s2)``
      -  Compares only the string payload bytes (caller must ensure equal lengths).
   -  -  ``zend_string_equals_cstr(s1, s2, l2)``
      -  Compares a ``zend_string`` with a ``char*`` buffer and explicit length.
   -  -  ``zend_string_equals_ci(s1, s2)``
      -  Case-insensitive full equality check.
   -  -  ``zend_string_equals_literal(str, literal)``
      -  Equality check against a string literal with compile-time literal length.
   -  -  ``zend_string_equals_literal_ci(str, literal)``
      -  Case-insensitive literal equality check.
   -  -  ``zend_string_starts_with(str, prefix)``
      -  Checks whether ``str`` begins with ``prefix``.
   -  -  ``zend_string_starts_with_cstr(str, prefix, prefix_length)``
      -  Prefix check against a ``char*`` buffer and explicit length.
   -  -  ``zend_string_starts_with_ci(str, prefix)``
      -  Case-insensitive prefix check for two ``zend_string`` values.
   -  -  ``zend_string_starts_with_cstr_ci(str, prefix, prefix_length)``
      -  Case-insensitive prefix check against a ``char*`` buffer.
   -  -  ``zend_string_starts_with_literal(str, prefix)``
      -  Prefix check against a string literal.
   -  -  ``zend_string_starts_with_literal_ci(str, prefix)``
      -  Case-insensitive prefix check against a string literal.

.. list-table:: Hashing APIs
   :header-rows: 1

   -  -  Function/Macro
      -  Description
   -  -  ``zend_string_hash_func(s)``
      -  Computes and stores the hash for ``s``.
   -  -  ``zend_string_hash_val(s)``
      -  Returns the cached hash if available, otherwise computes it.
   -  -  ``zend_hash_func(str, len)``
      -  Computes a hash for a raw ``char*`` buffer.
   -  -  ``zend_inline_hash_func(str, len)``
      -  Inline implementation of PHP's string hashing routine for ``char*`` buffers.
   -  -  ``zend_string_forget_hash_val(s)``
      -  Clears cached hash/derived flags after string contents change.

.. list-table:: Concatenation property macros
   :header-rows: 1

   -  -  Macro
      -  Description
   -  -  ``ZSTR_COPYABLE_CONCAT_PROPERTIES``
      -  Bitmask of string flags that can be preserved when concatenating strings.
   -  -  ``ZSTR_GET_COPYABLE_CONCAT_PROPERTIES(s)``
      -  Extracts copyable concatenation properties from one string.
   -  -  ``ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(s1, s2)``
      -  Extracts copyable properties shared by both input strings.
   -  -  ``ZSTR_COPY_CONCAT_PROPERTIES(out, in)``
      -  Copies concatenation properties from one input to the output string.
   -  -  ``ZSTR_COPY_CONCAT_PROPERTIES_BOTH(out, in1, in2)``
      -  Copies only properties that are set on both inputs.

.. list-table:: Interned string APIs
   :header-rows: 1

   -  -  Function
      -  Description
   -  -  ``zend_new_interned_string(s)``
      -  Interns ``s`` if possible and returns the interned instance.
   -  -  ``zend_string_init_interned(str, len, p)``
      -  Creates or fetches an interned string from raw bytes.
   -  -  ``zend_string_init_existing_interned(str, len, p)``
      -  Returns an interned string only if it already exists; does not create a new one.
   -  -  ``zend_interned_string_find_permanent(s)``
      -  Looks up ``s`` in the permanent interned string storage.
   -  -  ``zend_interned_strings_init()``
      -  Initializes interned string storage during engine startup.
   -  -  ``zend_interned_strings_dtor()``
      -  Destroys interned string storage during engine shutdown.
   -  -  ``zend_interned_strings_activate()``
      -  Activates request-local interned string state.
   -  -  ``zend_interned_strings_deactivate()``
      -  Deactivates request-local interned string state.
   -  -  ``zend_interned_strings_set_request_storage_handlers(...)``
      -  Installs callbacks that customize request interned string storage behavior.
   -  -  ``zend_interned_strings_switch_storage(request)``
      -  Switches between request and persistent interned string storage backends.

******************
 Interned strings
******************

Programs use some strings many times. For example, if your program declares a class called
``MyClass``, it would be wasteful to allocate a new string ``"MyClass"`` every time it is referenced
within your program. Instead, when repeated strings are expected, php-src uses a technique called
string interning. Essentially, this is just a simple `HashTable <todo>`__ where existing interned
strings are stored. When creating a new interned string, php-src first checks the interned string
buffer. If it finds it there, it can return a pointer to the existing string. If it doesn't, it
allocates a new string and adds it to the buffer.

.. code:: c

   zend_string *str1 = zend_new_interned_string(
       ZSTR_INIT_LITERAL("MyClass", /* persistent */ false));

   // In some other place entirely.
   zend_string *str2 = zend_new_interned_string(
       ZSTR_INIT_LITERAL("MyClass", /* persistent */ false));

   assert(ZSTR_IS_INTERNED(str1));
   assert(ZSTR_IS_INTERNED(str2));
   assert(str1 == str2);

Interned strings are *not* reference counted, as they are expected to live for the entire request,
or longer.

With opcache, this goes one step further by sharing strings across different processes. For example,
if you're using php-fpm with 8 workers, all workers will share the same interned strings buffer. It
gets a bit more complicated. During requests, no interned strings are actually created. Instead,
this is delayed until the script is persisted to shared memory. This means that
``zend_new_interned_string`` may not actually return an interned string if opcache is enabled.
Usually you don't have to worry about this.

Also, here are some API helpers that might be useful in stack allocation.

.. list-table:: Stack allocation helper macros
   :header-rows: 1

   -  -  Macro
      -  Description
   -  -  ``ZSTR_ALLOCA_ALLOC(str, l, use_heap)``
      -  Allocates a temporary string buffer using ``do_alloca``.
   -  -  ``ZSTR_ALLOCA_INIT(str, s, l, use_heap)``
      -  Same as ``ZSTR_ALLOCA_ALLOC``, then copies data from ``s`` and appends ``'\0'``.
   -  -  ``ZSTR_ALLOCA_FREE(str, use_heap)``
      -  Frees memory previously allocated with ``ZSTR_ALLOCA_ALLOC`` / ``ZSTR_ALLOCA_INIT``.
