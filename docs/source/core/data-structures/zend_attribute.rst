################
 zend_attribute
################

PHP attributes are stored in a dedicated structure ``zend_attribute``, which holds details of
attributes.

************
 definition
************

.. code:: c

   typedef struct _zend_attribute {
       zend_string *name;
       zend_string *lcname;
       uint32_t flags;
       uint32_t lineno;
       /* Parameter offsets start at 1, everything else uses 0. */
       uint32_t offset;
       uint32_t argc;
       zend_attribute_arg args[1];
   } zend_attribute;

Broadly, the attribute holds

-  the name of the attribute (``name``, ``lcname``)
-  metadata about the attribute (``flags``)
-  metadata about the attribute usage (``lineno``, ``offset``)
-  details of arguments (``argc``, ``args``)

******
 name
******

The name of the attribute is stored in the :doc:`./zend_string` ``name``. To make case-insensitive
comparisons easier, a lowercase version of the name is also saved in the :doc:`./zend_string`
``lcname``.

*******
 flags
*******

The ``flags`` field holds metadata about the internal details of how the attribute should be used.
The flags are made from

.. code:: c

   #define ZEND_ATTRIBUTE_PERSISTENT   (1<<0)
   #define ZEND_ATTRIBUTE_STRICT_TYPES (1<<1)

And are *not* the same as the flags in ``Attribute::$flags``.

**********
 metadata
**********

The ``name`` holds a :doc:`zend_string` with the name of the constant, to allow searching for
constants that have already been defined. This string is released when the constant itself is freed.

**********
 filename
**********

The ``lineno`` field holds the line number that the attribute is used on, for use in error messages.

The ``offset`` field is only used for attributes on parameters, and indicates *which* parameter the
attribute is applied to (since the attributes for a function's parameters are all stored together).

***********
 arguments
***********

The number of arguments that are passed to the attribute is stored in the ``argc`` field. But, since
attributes support the use of named parameters, storing just the parameters that were used is not
enough, the parameter names need to be stored too.

Those are stored in ``zend_attribute_arg`` objects:

.. code:: c

   typedef struct {
       zend_string *name;
       zval value;
   } zend_attribute_arg;

Where the ``name`` field stores the name used, and the :doc:`./zval` ``value`` field stores the
value.
