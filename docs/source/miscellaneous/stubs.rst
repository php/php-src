#######
 Stubs
#######

Stub files are pieces of plain PHP code which only contain declarations without actually runnable
code. A very basic stub looks like this:

.. code:: php

   /** @var string */
   const FOO = "foo";
   /** @var int */
   const BAR = "bar";

   class Foo {
       public function bar(): string {}
   }

   function foo(string $param): string {}

Any kind of symbols can be declared via stubs, and with the exception of disjunctive normal form
(DNF) types, basically any kind of type declaration is supported. Additional meta information can be
added either via PHPDoc or attributes. Namespaces can also be used by either adding a top-level
``namespace`` declaration or by using namespace blocks:

.. code:: php

   namespace {
       /** @var string */
       const FOO = "foo";
       /** @var string */
       const BAR = "bar";

       class Foo {
           public function bar(): string {}
       }
   }

   namespace Foo {
       function foo(string $param): string {}
   }

The above example declares the global constants and class ``Foo`` in the top-level namespace, while
``foo()`` will be available in the ``Foo`` namespace.

********************
 Using gen_stub.php
********************

By convention, stub files have a ``.stub.php`` extension. They are processed by
``build/gen_stub.php``: it uses PHP-Parser_ for parsing, then depending on the configuration and the
supplied arguments, it can generate various artifacts. The following sections will introduce these
capabilities.

.. _php-parser: https://github.com/nikic/PHP-Parser

*******************************
 Generating arginfo structures
*******************************

The original purpose of stubs was to make it easier to declare arginfo structures. Previously, one
had to manually use the different ``ZEND_BEGIN_ARG_* ... ZEND_END_ARG_INFO()`` macros. This was a
tedious and error-prone process, so being able to use pure PHP code based on which the C code can be
generated is a huge relief. The first example above results in the following arginfo file:

.. code:: c

   /* This is a generated file, edit the .stub.php file instead.
    * Stub hash: 3026b5948bc28345598b3dfa649e310d59f370a5 */

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_foo, 0, 1, IS_STRING, 0)
       ZEND_ARG_TYPE_INFO(0, param, IS_STRING, 0)
   ZEND_END_ARG_INFO()

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Foo_bar, 0, 0, IS_STRING, 0)
   ZEND_END_ARG_INFO()

Please note the hash above. It makes sure that stub files are not reprocessed unless there was any
actual change, or something requires it to be processed (e.g. regeneration was forced by using the
`-f` flag).

Another very important thing to keep in mind is that stub-based type declarations have to be in sync
with the parameter parsing code (``ZPP``). Even though only ZPP is run in case of release builds
when an internal function or method is invoked, (meaning that arginfo structures are only used for
reflection purposes), however, the result of ``ZPP`` as well as the actual type of the return value
is compared with the available reflection information in debug builds, and errors are raised in case
of any incompatibility. That's why only absolutely correct types should ever be declared in stubs.
For documentation purposes, PHPDoc can be used.

Since PHP 8.0, arginfo structures can also store default values in order to be used by
``ReflectionParameter::getDefaultValue()`` among some other use-cases. Default values may not only
be literals, but compile-time evaluable expressions, possibly containing references to constants.
Let's modify function ``foo()`` slightly in our original example by making ``$param`` an optional
parameter:

.. code:: php

   function foo(string $param = FOO . "bar"): string {}

This will result in the following arginfo:

.. code:: c

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_foo, 0, 0, IS_STRING, 0)
      ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, param, IS_LONG, 0, "FOO . \"bar\"")
   ZEND_END_ARG_INFO()

By default, referencing constants works as long as the constant is available in the same stub file.
If this is not possible by any reason, then the stub declaring the necessary constant should be
included:

.. code:: php

   // constants.stub.php

   /** @var string */
   const FOO = "foo";
   /** @var string */
   const BAR = "bar";

.. code:: php

   // example.stub.php

   require "constants.stub.php";

   function foo(string $param = FOO): string {}

Sometimes, arguments have to be passed by reference, or by using the `ZEND_SEND_PREFER_REF` flag.
Passing by reference is trivial to declare by the usual syntax, while the latter can be achieved by
using the ``@prefer-ref`` PHPDoc tag:

.. code:: php

   /**
    * @param string $param1
    * @prefer-ref $param2
    */
   function foo(&$param1, string $param2): string {}

*****************************
 Generating function entries
*****************************

Besides arginfo structures, function entries themselves can also be generated via stubs. In order to
make this work, the file-level ``@generate-function-entries`` PHPDoc tag has to be added to our
stub:

.. code:: php

   /** @generate-function-entries */

   class Foo {
       public function bar(): string {}
   }

   function foo(string $param): string {}

Now, the following C code is added to our original arginfo file:

.. code:: c

   /* ... */

   ZEND_FUNCTION(foo);
   ZEND_METHOD(Foo, bar);

   static const zend_function_entry ext_functions[] = {
       ZEND_FE(foo, arginfo_foo)
       ZEND_FE_END
   };

   static const zend_function_entry class_Foo_methods[] = {
       ZEND_ME(Foo, bar, arginfo_class_Foo_bar, ZEND_ACC_PUBLIC)
       ZEND_FE_END
   };

The ``ext_functions`` variable is to be passed for the ``functions`` member of `zend_module_entry`,
while ``class_Foo_methods`` can be used when registering the ``Foo`` class:

.. code:: c

   INIT_CLASS_ENTRY(ce, "Foo", class_Foo_methods);

Function entries may make use of extra meta information passed via the following PHPDoc tags:

-  ``@deprecated``: Triggers the usual deprecation notice when the function/method is called.

-  ``@alias``: If a function/method is an alias of another function/method, then the aliased
   function/method name has to be provided as value. E.g. function ``sizeof()` has the ``@alias
   count`` annotation.

-  ``@implementation-alias``: This is very similar to ``@alias`` with some semantic difference:
   these aliases exists purely to avoid duplicating some code, but there is no other connection
   between the alias and the aliased function or method. A notable example is ``Error::getCode()``
   which has the ``@implementation-alias Exception::getCode`` annotation. The difference between
   ``@alias`` and ``@implementation-alias`` is very nuanced and is only observable in the manual.

-  ``@tentative-return-type``: By using this annotation, the return type declaration is reclassified
   as a `tentative return type`_.

-  ``@genstubs-expose-comment-block``: By adding this annotation at the beginning of a docblock, the
   content of the docblock will be exposed for `ReflectionFunctionAbstract::getDocComment()`. This
   feature is only available as of PHP 8.4.0.

.. _tentative return type: https://wiki.php.net/rfc/internal_method_return_types

**************************
 Generating class entries
**************************

Until now, we only covered how to deal with functions. But as mentioned in the beginning, stubs can
declare any kind of symbols. In order to generate the code which is necessary for registering
constants, classes, properties, enums, and traits, the ``@generate-class-entries`` file-level PHPDoc
tag has to be added to the stub. Using ``@generate-class-entries`` automatically implies
``@generate-function-entries```, so there is no use of adding the latter.

Given the following stub:

.. code:: php

   /** @generate-class-entries */

   enum Number: string {
       /** @var string */
       public const ONE = "one";

       case One = Number::ONE;
   }

   class Foo {
       /** @cvalue M_PI */
       public const float PI = UNKNOWN;

       public readonly string $prop;
   }

The following arginfo file is generated:

.. code:: c

   static const zend_function_entry class_Number_methods[] = {
       ZEND_FE_END
   };

   static const zend_function_entry class_Foo_methods[] = {
       ZEND_FE_END
   };

   static zend_class_entry *register_class_Number(void)
   {
       zend_class_entry *class_entry = zend_register_internal_enum("Number", IS_STRING, class_Number_methods);

       /*  ... */

       return class_entry;
   }

   static zend_class_entry *register_class_Foo(void)
   {
       zend_class_entry ce, *class_entry;

       /*  ... */

       return class_entry;
   }

We can disregard the implementation details of the ``register_class_*()`` functions, and directly
use them to register enum ``Number`` and class ``Foo``:

.. code:: c

   zend_class_entry *number_ce = register_class_Number();
   zend_class_entry *foo_ce = register_class_Foo(zend_standard_class_def);

It's worth to note that the class entry of any dependency (e.g. the parent class or implemented
interfaces) has to be manually passed to the register function: in this specific case, the class
entry for ``stdClass`` (``zend_standard_class_def``) was passed.

Just like functions and methods, classes also support some meta information passed via PHPDoc tags:

-  ``@deprecated``: triggers a deprecation notice when the class is used

-  ``@strict-properties``: adds the ``ZEND_ACC_NO_DYNAMIC_PROPERTIES`` flag for the class (as of PHP
   8.0), which disallow dynamic properties.

-  ``@not-serializable``: adds the ``ZEND_ACC_NOT_SERIALIZABLE`` flag for the class (as of PHP 8.1),
   which prevents the serialization of the class.

-  ``@genstubs-expose-comment-block``: By adding this tag at the beginning of a docblock, the
   content of the docblock will be exposed for `ReflectionClass::getDocComment()`. This feature is
   only available as of PHP 8.4.0.

********************************************
 Generating global constants and attributes
********************************************

We have not covered so far how to register global constants and attributes for functions. Slightly
surprisingly, the ``/** @generate-class-entries */``` file-level PHPDoc tag is necessary for it to
work, even though, neither of them relate to classes. That's also why the C code which registers
these symbols takes place in a function called ``register_{{ STUB FILE NAME }}_symbols()```. Given
the following ``example.stub.php``` file:

.. code:: php

   <?php

   /** @generate-class-entries */

   /** @var string */
   const FOO = "foo";

   /**
    * @var float
    * @cvalue M_PI
    */
   const BAR = UNKNOWN;

   function foo(#[\SensitiveParameter] string $param): string {}

The following C function will be generated in order to register the two global constants and the
attribute:

.. code:: c

   static void register_example_symbols(int module_number)
   {
       /*  ... */
   }

Similarly to class registration functions, ``register_example_symbols()`` also has to be called
during the module initialization process (``MINIT``), and the ``module_number`` variable has to be
passed to it:

.. code:: c

   PHP_MINIT_FUNCTION(example)
   {
       register_example_symbols(module_number);

       return SUCCESS;
   }

Constant registration needs more elaboration: no matter if the constant type can be inferred from
the value, it always has to be provided via the ``@var`` PHPDoc tag. For typed class constants,
``@var`` can be omitted, and the type declaration will serve for this purpose.

In lots of cases, constants have a value which comes from a 3rd party library, or it may also be
possible that the value is a C expression (e.g. think about a combination of bit flags). Sometimes
the exact value is not even known, because it depends on the system. In these cases, it's better not
to duplicate the exact value in the stub, but the C value should be referenced instead. That's when
the ``UNKNOWN`` constant value along with the ``@cvalue`` PHPDoc tag is useful: this combination can
be used to refer to a C value (see constant ``BAR`` in the last example).

Constants can take the following extra meta information passed via PHPDoc tags:

-  ``@deprecated``: Triggers a deprecation notice when the constant is used.

-  ``@genstubs-expose-comment-block``: By adding this tag at the beginning of a docblock, the
   content of the docblock will be exposed for `ReflectionClass::getDocComment()`. This feature is
   only available as of PHP 8.4.0.

************************************
 Maintaining backward compatibility
************************************

While php-src itself processes the built-in stubs with only the latest version of ``gen_stub.php``
which is available in a specific PHP version, the same is not true for 3rd party extensions: their
stubs need to keep backward compatibility with older PHP versions even when using the latest version
of ``gen_stub.php``. Achieving this is not straightforward, since stubs may get new features which
are unavailable in earlier PHP versions, or ABI compatibility breaks may happen between minor
releases. Not to mention the fact that PHP 7.x versions are substantially different from nowadays'
PHP versions.

That's why it is useful to be able to declare the backward compatibility expectations of a stub.
This is possible via using the ``@generate-legacy-arginfo`` file-level PHPDoc tag. If no value is
passed to the annotation, the generated arginfo code will be compatible with PHP 7.0. In order to
achieve this, an additional arginfo file is generated with a ``_legacy_arginfo.h`` suffix besides
the regular one. Symbols declared by a legacy arginfo file will miss any type information and any
PHP 8.x features. An extension author can then include the proper arginfo file depending on which
PHP version the extension is built for.

When ``@generate-legacy-arginfo`` is passed a PHP version ID (``80000`` for PHP 8.0, ``80100`` for
PHP PHP 8.1, ``80200`` for PHP 8.2, ``80300`` for PHP 8.3, and ``80400`` for PHP 8.4), then only one
arginfo file is going to be generated, and ``#if`` prepocessor directives will ensure compatibility
with all the required PHP versions.

Let's add the PHP 8.0 compatibility requirement to the slightly modified version of our previous
example:

.. code:: php

   /**
    * @generate-class-entries
    * @generate-legacy-arginfo 80000
    */

   enum Number {
       case One;
   }

   class Foo {
       public readonly string $prop;

       public function foo(string $param): string {}
   }

Then notice the ``#if (PHP_VERSION_ID >= ...)`` conditions in the generated arginfo file:

.. code:: c

   /*  ... */

   #if (PHP_VERSION_ID >= 80100)
   static zend_class_entry *register_class_Number(void)
   {
       /*  ... */
   }
   #endif

   static zend_class_entry *register_class_Foo(void)
   {
       zend_class_entry ce, *class_entry;

       INIT_CLASS_ENTRY(ce, "Foo", class_Foo_methods);
       class_entry = zend_register_internal_class_ex(&ce, NULL);

       zval property_prop_default_value;
       ZVAL_UNDEF(&property_prop_default_value);
       zend_string *property_prop_name = zend_string_init("prop", sizeof("prop") - 1, 1);
   #if (PHP_VERSION_ID >= 80100)
       zend_declare_typed_property(class_entry, property_prop_name, &property_prop_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
   #elif (PHP_VERSION_ID >= 80000)
       zend_declare_typed_property(class_entry, property_prop_name, &property_prop_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
   #endif
       zend_string_release(property_prop_name);

       return class_entry;
   }

The preprocessor conditions are necessary because ``enum``s and ``readonly`` properties are PHP 8.1
features and consequently, they don't exist in PHP 8.0. Therefore, the registration of ``Number`` is
completely omitted, while the ``readonly`` flag is not added for ``Foo::$prop`` below PHP 8.1
versions.

******************************************
 Generating information for the optimizer
******************************************

A list of functions is maintained for the optimizer in ``Zend/Optimizer/zend_func_infos.h``
containing extra information about the return type and the cardinality of the return value. These
pieces of information can enable more accurate optimizations (i.e. better type inference).
Previously, the file was maintained manually, however since PHP 8.1, ``gen_stub.php`` can take care
of this task when the ``--generate-optimizer-info`` option is passed to it.

A function is added to ``zend_func_infos.h`` if either the ``@return`` or the ``@refcount`` PHPDoc
tag supplies more information than what is available based on the return type declaration. By
default, scalar return types have a ``refcount`` of ``0``, while non-scalar values are ``N``. If a
function can only return newly created non-scalar values, its ``refcount`` can be set to ``1``.

An example from the built-in functions:

.. code:: php

   /**
    * @return array<int, string>
    * @refcount 1
    */
   function get_declared_classes(): array {}

Please note that the feature is only available for built-in stubs inside php-src, since currently
there is no way to provide the function list for the optimizer other than overwriting
``zend_func_infos.h`` directly.

Additionally, functions can be evaluated at compile-time if their arguments are known in
compile-time and their behavior if free from side-effects as well as it is not affected by the
global state. Until PHP 8.2, a list of such functions was maintained manually in the optimizer.
However, since PHP 8.2, the ``@compile-time-eval`` PHPDoc tag can be applied to any functions which
conform to the above restrictions in order for them to qualify as evaluable at compile-time. The
feature internally works by adding the ``ZEND_ACC_COMPILE_TIME_EVAL`` function flag.

As of PHP 8.4, the concept of arity-based frameless functions was introduced. This is another
optimization technique, which results in faster internal function calls by eliminating unnecessary
checks for the number of passed parameters (if the number of passed arguments is known at
compile-time).

In order to take advantage of frameless functions, the ``@frameless-function`` PHPDoc tag has to be
provided along with some configuration. Since currently only arity-based optimizations are
supported, the following should be provided: ``@frameless-function {"arity": NUM}``, where ``NUM``
is the number of parameters for which a frameless function is available. Let's see the stub of
``in_array()`` as an example:

.. code:: php

   /**
    * @compile-time-eval
    * @frameless-function {"arity": 2}
    * @frameless-function {"arity": 3}
    */
   function in_array(mixed $needle, array $haystack, bool $strict = false): bool {}

Apart from being compile-time evaluable, it has a frameless function counterpart for both the 2 and
the 3-parameter signatures:

.. code:: c

   /* The regular in_array() function */
   PHP_FUNCTION(in_array)
   {
       php_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
   }

   /* The frameless version of the in_array() function when 2 arguments are passed */
   ZEND_FRAMELESS_FUNCTION(in_array, 2)
   {
       zval *value, *array;

       Z_FLF_PARAM_ZVAL(1, value);
       Z_FLF_PARAM_ARRAY(2, array);

       _php_search_array(return_value, value, array, false, 0);

   flf_clean:;
   }

   /* The frameless version of the in_array() function when 3 arguments are passed */
   ZEND_FRAMELESS_FUNCTION(in_array, 3)
   {
       zval *value, *array;
       bool strict;

       Z_FLF_PARAM_ZVAL(1, value);
       Z_FLF_PARAM_ARRAY(2, array);
       Z_FLF_PARAM_BOOL(3, strict);

       _php_search_array(return_value, value, array, strict, 0);

   flf_clean:;
   }

**************************************
 Generating signatures for the manual
**************************************

Theoretically, the manual should reflect the exact same signatures which are represented by the
stubs. This is not exactly the case yet for built-in symbols, but ``gen_stub.php`` have multiple
features to automate the process of synchronization.

First of all, newly added functions or methods can be documented by providing the
``--generate-methodsynopses`` option. E.g. running ``./build/gen_stub.php --generate-methodsynopses
./ext/mbstring ../doc-en/reference/mbstring`` will create a dedicated page for each ``ext/mbstring``
function which is not yet documented, saving them into the
``../doc-en/reference/mbstring/functions`` directory. Since these are stub pages, many of the
sections are empty by default, so the relevant descriptions have to be added, while the irrelevant
ones have to be removed.

For functions or methods which are already available in the manual, the documented signatures can be
updated by providing the ``--replace-methodsynopses`` option. E.g. running ``./build/gen_stub.php
--replace-methodsynopses ./ ../doc-en/`` will update all the function or method signatures in the
English documentation whose stub counterpart is found.

Class signatures can be updated in the manual by providing the ``--replace-classsynopses`` option.
E.g. running ``./build/gen_stub.php --replace-classsynopses ./ ../doc-en/`` will update all the
class signatures in the English documentation whose stub counterpart is found.

If a symbol is not intended to be documented, the ``@undocumentable`` PHPDoc tag should be added to
it. Doing so will prevent any documentation to be created for the given symbol. In order not to add
a whole stub file to the manual, the PHPDoc tag should be applied to the file itself. These
possibilities are useful for symbols which exist only for testing purposes (e.g. the ones declared
for ``ext/zend_test``), or by some other reason documentation is not possible.

************
 Validation
************

It's possible to validate whether the alias function/method signatures are correct by providing the
``--verify`` flag to ``gen_stub.php``. Normally, an alias function/method should have the exact same
signature as its aliased function/method counterpart has apart from the name. In some cases this is
not achievable by some reason (i.e. ``bzwrite()`` is an alias of ``fwrite()``, but the name of the
first parameter is different because the resource types differ). In order to suppress the error when
the check is false positive, the ``@no-verify`` PHPDoc tag should be applied to the alias:

.. code:: php

   /**
    * @param resource $bz
    * @implementation-alias fwrite
    * @no-verify Uses different parameter name
    */
   function bzwrite($bz, string $data, ?int $length = null): int|false {}

Besides aliases, the contents of the documentation can also be validated by providing the
``--verify-manual`` option to ``gen_stub.php`` along with the path of the manual as the last
argument: e.g. ``./build/gen_stub.php --verify-manual ./ ../doc-en/`` when validation is based on
all stubs in ``php-src`` and the English documentation is available in the parent directory.

When this feature is used, the following validations are performed:

-  Detecting missing global constants
-  Detecting missing classes
-  Detecting missing methods
-  Detecting incorrectly documented alias functions or methods

**********************
 Parameter statistics
**********************

A less commonly used feature of ``gen_stub.php`` is to count how many times a parameter name occurs
in the codebase: ``./build/gen_stub.php --parameter-stats``. The result is a JSON object containing
the parameter names and the number of their occurrences in descending order.
