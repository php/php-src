###############
 zend_constant
###############

PHP constants (referring to non-class constants) are stored in a dedicated structure
``zend_constant``, which holds both the value of the constant and details for using it.

************
 definition
************

.. code:: c

   typedef struct _zend_constant {
       zval value;
       zend_string *name;
       zend_string *filename;
       HashTable *attributes;
   } zend_constant;

The ``value`` field stores both the value itself and some metadata. The ``name`` and ``filename``
store the name of the constant and the name of the file in which it was defined. The ``attributes``
field stores the attributes applied to the constant.

*******
 value
*******

The value of the constant is stored in the :doc:`./zval` ``value``. However, since the ``zval``
structure has extra space, for constants this is used to store both the number of the module that
the constant was defined in, and a combination of the flags that affect the usage of the constant.

This extra information is placed in the ``uint32_t`` field ``value.u2.constant_flags``.

The bottom 16 bits are used to hold flags about the constant

.. code:: c

   #define CONST_PERSISTENT     (1<<0) /* Persistent */
   #define CONST_NO_FILE_CACHE  (1<<1) /* Can't be saved in file cache */
   #define CONST_DEPRECATED     (1<<2) /* Deprecated */
   #define CONST_OWNED          (1<<3) /* constant should be destroyed together
                                            with class */

These bottom 16 bits can be accessed with the ``ZEND_CONSTANT_FLAGS()`` macro, which is given a
``zend_constant`` pointer as a parameter.

On the other hand, the top 16 bits are used to store the number of the PHP module that registered
the constant. For constants defined by the user, the module number stored will be
``PHP_USER_CONSTANT``. This module number can be accessed with the ``ZEND_CONSTANT_MODULE_NUMBER()``
macro, which is likewise given a ``zend_constant`` pointer as a parameter.

******
 name
******

The ``name`` holds a :doc:`zend_string` with the name of the constant, to allow searching for
constants that have already been defined. This string is released when the constant itself is freed.

**********
 filename
**********

The ``filename`` holds another ``zend_string`` with the name of the file in which the constant was
defined, or ``NULL`` if not defined userland code. This field provides the foundation for the PHP
method ``ReflectionConstant::getFileName()``.

************
 attributes
************

The ``attributes`` holds a ``HashTable`` (essentially an array) with the details of the attributes
that were applied to the constant. Note that attributes can only be added to constants declared at
compile time via ``const``, e.g. ``const EXAMPLE = 123``, not those declared at runtime, e.g.
``define( 'EXAMPLE', 123 );``.
