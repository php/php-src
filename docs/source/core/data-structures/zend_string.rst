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
to be expanded arbitrarily. In this case, the size of ``zend_string`` depends on the strings length,
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

The string API is defined in ``Zend/zend_string.h``. It provides a number of functions for creating
new strings.

.. list-table:: ``zend_string`` creation
   :header-rows: 1

   -  -  Function/Macro [#persistent]_
      -  Description

   -  -  ``ZSTR_INIT(s, p)``
      -  Creates a new string from a string literal.

   -  -  ``zend_string_init(s, l, p)``
      -  Creates a new string from a character buffer.

   -  -  ``zend_string_alloc(l, p)``
      -  Creates a new string of a given length without initializing its content.

   -  -  ``zend_string_concat2(s1, l1, s2, l2)``
      -  Creates a non-persistent string by concatenating two character buffers.

   -  -  ``zend_string_concat3(...)``
      -  Same as ``zend_string_concat2``, but for three character buffers.

   -  -  ``ZSTR_EMPTY_ALLOC()``
      -  Gets an immutable, empty string. This does not allocate memory.

   -  -  ``ZSTR_CHAR(char)``
      -  Gets an immutable, single-character string. This does not allocate memory.

   -  -  ``ZSTR_KNOWN(ZEND_STR_const)``

      -  Gets an immutable, predefined string. Used for string common within PHP itself, e.g.
         ``"class"``. See ``ZEND_KNOWN_STRINGS`` in ``Zend/zend_string.h``. This does not allocate
         memory.

.. [#persistent]

   ``s`` = ``zend_string``, ``l`` = ``length``, ``p`` = ``persistent``.

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
      -  Computes the string has if it hasn't already been, and returns it.

   -  -  ``ZSTR_H``
      -  \-
      -  Returns the string hash. This macro assumes that the hash has already been computed.

.. list-table:: Reference counting macros
   :header-rows: 1

   -  -  Macro
      -  Description

   -  -  ``zend_string_copy(s)``
      -  Increases the reference count and returns the same string. The reference count is not
         increased if the string is interned.

   -  -  ``zend_string_release(s)``
      -  Decreases the reference count and frees the string if it goes to 0.

   -  -  ``zend_string_dup(s, p)``
      -  Creates a true copy of the string in a new allocation, except if the string is interned.

   -  -  ``zend_string_separate(s)``
      -  Duplicates the string if the reference count is greater than 1. See
         :doc:`./reference-counting` for details.

   -  -  ``zend_string_realloc(s, l, p)``

      -  Changes the size of the string. If the string has a reference count greater than 1 or if
         the string is interned, a new string is created. You must always use the return value of
         this function, as the original array may have been moved to a new location in memory.

There are various functions to compare strings. The ``zend_string_equals`` function compares two
strings in full, while ``zend_string_starts_with`` checks whether the first argument starts with the
second. There are variations for ``_ci`` and ``_literal``, i.e. case-insensitive comparison and
literal strings, respectively. We won't go over all variations here, as they are straightforward to
use.

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
