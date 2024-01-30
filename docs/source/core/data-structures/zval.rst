######
 zval
######

PHP is a dynamic language. A variable can typically contain a value of any type, and the type of the
variable may even change during the execution of the program. Under the hood, this is implemented
through the ``zval`` struct. It is one of the most important data structures in php-src. It is
implemented as a "tagged union", meaning it stores what type of value it contains, and the value
itself. Let's look at the type first.

************
 zval types
************

.. code:: c

   #define IS_UNDEF     0 /* A variable that was never written to. */
   #define IS_NULL      1
   #define IS_FALSE     2
   #define IS_TRUE      3
   #define IS_LONG      4 /* An integer value. */
   #define IS_DOUBLE    5 /* A floating point value. */
   #define IS_STRING    6
   #define IS_ARRAY     7
   #define IS_OBJECT    8
   #define IS_RESOURCE  9
   #define IS_REFERENCE 10

These simple integer constants determine what value is currently stored in a variable. If you are a
PHP developer, these types should sound fairly familiar. They are pretty much an exact reflection of
the types you may use in regular PHP code. One small oddity is that ``IS_FALSE`` and ``IS_TRUE`` are
implemented as separate types, instead of as a ``IS_BOOL`` type.

Some of these types are self-contained, they don't store any auxiliary data. This includes
``IS_UNDEF``, ``IS_NULL``, ``IS_FALSE`` and ``IS_TRUE``. For the rest of the types, we are going to
require some additional memory to store the actual value of the variable.

************
 zend_value
************

.. code:: c

   typedef union _zend_value {
       zend_long         lval; /* long value, i.e. int. */
       double            dval; /* double value, i.e. float. */
       zend_refcounted  *counted;
       zend_string      *str;
       zend_array       *arr;
       zend_object      *obj;
       zend_resource    *res;
       zend_reference   *ref;
       // Less important for now.
       zend_ast_ref     *ast;
       zval             *zv;
       void             *ptr;
       zend_class_entry *ce;
       zend_function    *func;
       struct {
           uint32_t w1;
           uint32_t w2;
       } ww;
   } zend_value;

A C union is a data type that may store any one of its members at a time, by being (at least) as big
as its biggest member. For example, ``zend_value`` may store the ``lval`` member, or the ``dval``
member, but never both at the same time. However, it doesn't know which member is being stored.
Remembering this is our job, and that's exactly what the ``IS_*`` constants are for.

The top members of ``zend_value`` mostly mirror the ``IS_*`` constants, with the exception of
``counted``. ``counted`` polymorphically refers to any `reference counted <todo>`__ value, including
strings, arrays, objects, resources and references. ``null`` and ``bool`` are missing from
``zend_value`` because their types are self-contained.

The rest of the fields aren't important for now.

******
 zval
******

Together, the value and the tag make up the ``zval``, along with some other fields. It may look
intimidating at first. We'll go over it step by step.

.. code:: c

   typedef struct _zval_struct zval;

   struct _zval_struct {
       zend_value value;
       union {
           uint32_t type_info;
           struct {
               ZEND_ENDIAN_LOHI_3(
                   uint8_t type, /* active type */
                   uint8_t type_flags,
                   union {
                       uint16_t extra; /* not further specified */
                   } u)
           } v;
       } u1;
       union {
           uint32_t next;           /* hash collision chain */
           uint32_t cache_slot;     /* cache slot (for RECV_INIT) */
           uint32_t opline_num;     /* opline number (for FAST_CALL) */
           uint32_t lineno;         /* line number (for ast nodes) */
           uint32_t num_args;       /* arguments number for EX(This) */
           uint32_t fe_pos;         /* foreach position */
           uint32_t fe_iter_idx;    /* foreach iterator index */
           uint32_t guard;          /* recursion and single property guard */
           uint32_t constant_flags; /* constant flags */
           uint32_t extra;          /* not further specified */
       } u2;
   };

``zval.value`` reserves space for the actual variable data, as discussed above.

``zval.u1`` stores the variable type, the given ``IS_*`` constant, along with some other flags. It's
definition looks a bit complicated. You can think of the entire field as a 4 bit integer, split into
3 parts. ``v.type`` stores the actual variable type, ``v.type_flags`` is used for some `reference
counting <todo>`__ flags, and ``v.u.extra`` is pretty much unused.

``zval.u2`` defines some more storage for various contexts that is often unoccupied. It's there
because the memory would otherwise be wasted due to padding, so we may as well make use of it. We'll
go over the relevant ones in their corresponding chapters.

********
 Macros
********

The fields in ``zval`` should never be accessed directly. Instead, there are a plethora of macros to
access them, concealing some of the implementation details of the ``zval`` struct. For many macros,
there's a ``_P``-suffixed variant that performs the same operation on a pointer to the given
``zval``.

.. list-table:: ``zval`` macros
   :header-rows: 1

   -  -  Macro
      -  Description
   -  -  ``Z_TYPE[_P]``
      -  Access the ``zval.u1.v.type`` part of the type flags, containing the ``IS_*`` type.
   -  -  ``Z_LVAL[_P]``
      -  Access the underlying ``int`` value.
   -  -  ``Z_DVAL[_P]``
      -  Access the underlying ``float`` value.
   -  -  ``Z_STR[_P]``
      -  Access the underlying ``zend_string`` pointer.
   -  -  ``Z_STRVAL[_P]``
      -  Access the strings raw ``char *`` pointer.
   -  -  ``Z_STRLEN[_P]``
      -  Access the strings length.
   -  -  ``ZVAL_COPY_VALUE(t, s)``
      -  Copy one ``zval`` to another, including type and value.
   -  -  ``ZVAL_COPY(t, s)``
      -  Same as ``ZVAL_COPY_VALUE``, but if the value is reference counted, increase the counter.

..
   _todo: There are many more.

******************
 Other zval types
******************

``zval``\ s are sometimes used internally with types that don't exist in userland.

.. code:: c

   #define IS_CONSTANT_AST 11
   #define IS_INDIRECT     12
   #define IS_PTR          13
   #define IS_ALIAS_PTR    14
   #define _IS_ERROR       15

``IS_CONSTANT_AST`` is used to represent constant values (the right hand side of ``const``,
property/parameter initializers, etc.) before they are evaluated. The evaluation of a constant
expression is not always possible during compilation, because they may contain references to values
only available at runtime. Until that evaluation is possible, the constants contain the AST of the
expression rather than the concrete values. Check the `parser <todo>`__ chapter for more information
on ASTs. When this flag is set, the ``zval.value.ast`` union member is set accordingly.

``IS_INDIRECT`` indicates that the ``zval.value.zv`` member is populated. This field stores a
pointer to some other ``zval``. This type is mainly used in two situations, namely for intermediate
values between ``FETCH`` and ``ASSIGN`` instructions, and for the sharing of variables in the symbol
table.

..
   _todo: There are many more.

``IS_PTR`` is used for pointers to arbitrary data. Most commonly, this type is used internally for
``HashTable``, as ``HashTable`` may only store ``zval`` values. For example, ``EG(class_table)``
represents the class table, which is a hash map of class names to the corresponding
``zend_class_entry``, representing the class. The same goes for functions and many other data types.
``IS_ALIAS_PTR`` is used for class aliases registered via ``class_alias``. Essentially, it just
allows differencing between members in the class table that are aliases, or actual classes.
Otherwise, it is essentially the same as ``IS_PTR``. Arbitrary data is accessed through
``zval.value.ptr``, and casted to the correct type depending on context. If ``ptr`` stores a class
or function, the ``zval.value.ce`` or ``zval.value.func`` fields may be used, respectively.

``_IS_ERROR`` is used as an error value for some `object handlers <todo>`__. It is described in more
detail in its own chapter.

.. code:: c

   /* Fake types used only for type hinting.
    * These are allowed to overlap with the types below. */
   #define IS_CALLABLE 12
   #define IS_ITERABLE 13
   #define IS_VOID     14
   #define IS_STATIC   15
   #define IS_MIXED    16
   #define IS_NEVER    17

   /* used for casts */
   #define _IS_BOOL   18
   #define _IS_NUMBER 19

These flags are never actually stored in ``zval.u1``. They are used for type hinting and in the
`object handler <todo>`__ API.

This only leaves the ``zval.value.ww`` field. In short, this field is used on 32-bit platforms when
copying data from one ``zval`` to another. Normally, ``zval.value.counted`` is copied as a generic
value, no matter what the actual underlying type is. ``zend_value`` always consists of 8 bytes due
to the ``double`` field. Pointers, however, consist only of 4. Because we would otherwise miss the
other 4 bytes, they are copied manually using ``z->value.ww.w2 = _w2;``. This happens in the
``ZVAL_COPY_VALUE_EX`` macro, you won't ever have to care about this.
