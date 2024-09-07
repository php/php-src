#######
 Stubs
#######

Stub files are pieces of PHP code which only contain declarations. They do not include runnable
code, but instead contain empty function and method bodies. A very basic stub looks like this:

.. code:: php

   <?php
   /** @var string */
   const ANIMAL = "Elephant";
   /** @var float */
   const WEIGHT = 6.8;

   class Atmopshere {
       public function calculateBar(): float {}
   }

   function fahrenheitToCelcius(float $fahrenheitToCelcius): float {}

Any kind of symbol can be declared via stubs. Every type can be used, with the exception of
disjunctive normal form (DNF) types. Additional meta information can be added via PHPDoc blocks or
PHP attributes. Namespaces can also be used by adding a top-level ``namespace`` declaration or by
using namespace blocks:

.. code:: php

   <?php
   namespace {
       /** @var string */
       const ANIMAL = "Elephant";
       /** @var float */
       const WEIGHT_TON = 6.8;

       class Atmopshere {
           public function calculateBar(): float {}
       }
   }

   namespace Algorithms {
       function fahrenheitToCelcius(float $fahrenheit): float {}
   }

The above example declares the global constants ``ANIMAL`` and ``WEIGHT_TON``, and the class
``Atmopshere`` in the top-level namespace. The ``fahrenheitToCelcius()`` function is declared to be
in the ``Algorithms`` namespace.

********************
 Using gen_stub.php
********************

Stub files have the ``.stub.php`` extension by convention.

They are processed by ``build/gen_stub.php``, which uses PHP-Parser_ for parsing. Depending on the
configuration and the supplied arguments, it can generate various artefacts.

The following sections will introduce these capabilities.

.. _php-parser: https://github.com/nikic/PHP-Parser

*******************************
 Generating arginfo Structures
*******************************

The purpose of stubs files is to make it easier to declare arginfo structures, validate parameters
parsing declarations, and maintain documentation.

Previously, you had to manually use the different ``ZEND_BEGIN_ARG_* ... ZEND_END_ARG_INFO()``
macros. This is a tedious and error-prone process. Being able to use pure PHP code on which the C
code can be generated is a huge benefit.

The arginfo file matching our first example looks like:

.. code:: c

   /* This is a generated file, edit the .stub.php file instead.
    * Stub hash: e4ed788d54a20272a92a3f6618b73d48ec848f97 */

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fahrenheitToCelcius, 0, 1, IS_DOUBLE, 0)
       ZEND_ARG_TYPE_INFO(0, fahrenheitToCelcius, IS_DOUBLE, 0)
   ZEND_END_ARG_INFO()

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Atmopshere_calculateBar, 0, 0, IS_DOUBLE, 0)
   ZEND_END_ARG_INFO()

The hash that is included in the file makes sure that stub files are not reprocessed unless the stub
file was modified, or something requires it to be processed (e.g. regeneration was forced by using
the ``-f`` flag).

Another thing to keep in mind is that stub-based type declarations have to be in sync with the
parameter parsing code in the PHP functions through ``ZEND_PARSE_PARAMETERS_*`` macros (``ZPP``).

In release builds, the arginfo structures are only used with Reflection.

In debug builds, PHP will compare arginfo structures against ZPP macros to ensure that the stubs and
actual data matches for both arguments and return types. If they do not, an error is generated.
Therefore it is important that you declare the right types in your stub files.

For documentation purposes, PHPDoc can be used.

Since PHP 8.0, arginfo structures can also contain default values. These can for example be used by
``ReflectionParameter::getDefaultValue()``.

Besides constant literals, default values can also contain compile-time evaluable expressions, and
contain references to constants.

In the example below, we define a function with an optional argument, referencing a constant:

.. code:: php

   <?php
   /** @var string */
   const ANIMAL = "Elephant";

   function formatName(string $defaultName = ANIMAL . " Mc" . ANIMAL . "Face"): string {}

This will result in the following arginfo:

.. code:: c

   /* This is a generated file, edit the .stub.php file instead.
    * Stub hash: a9685164284e73f47b15838122b631ebdfef23d6 */

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_formatName, 0, 0, IS_STRING, 0)
       ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defaultName, IS_STRING, 0, "ANIMAL . \" Mc\" . ANIMAL . \"Face\"")
   ZEND_END_ARG_INFO()

You can only use constants as long as they are defined in the same stub file.

If this is not possible, then the stub declaring the constant should be included with ``require``:

.. code:: php

   // constants.stub.php
   <?php
   /** @var string */
   const ANIMAL = "Elephant";

.. code:: php

   // example.stub.php
   <?php
   require "constants.stub.php";

   function foo(string $param = ANIMAL): string {}

Sometimes arguments have to be passed by reference, or by using the `ZEND_SEND_PREFER_REF` flag.

To signal parsing by reference, use the usual ``&`` syntax.

To include the ``ZEND_SEND_PREFER_REF`` flag, use the ``@prefer-ref`` PHPDoc tag:

.. code:: php

   <?php
   /**
    * @param array $herd
    * @prefer-ref $elephantName
    */
   function addElephantsToHerd(&$herd, string $elephantName): string {}

This results in the following arginfo file:

.. code:: c

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_addElephantsToHerd, 0, 2, IS_STRING, 0)
       ZEND_ARG_INFO(1, herd)
       ZEND_ARG_TYPE_INFO(ZEND_SEND_PREFER_REF, elephantName, IS_STRING, 0)
   ZEND_END_ARG_INFO()

*****************************
 Generating Function Entries
*****************************

Besides arginfo structures, function entries themselves can also be generated via stubs.

In order to generate these, add the file-level ``@generate-function-entries`` PHPDoc tag:

.. code:: php

   <?php
   /** @generate-function-entries */

   class Atmosphere {
      public function calculateBar(): float {}
   }

   function fahrenheitToCelcius(float $fahrenheit): float {}

Now, the following C code is generated:

.. code:: c

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_fahrenheitToCelcius, 0, 1, IS_DOUBLE, 0)
       ZEND_ARG_TYPE_INFO(0, fahrenheit, IS_DOUBLE, 0)
   ZEND_END_ARG_INFO()

   ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Atmosphere_calculateBar, 0, 0, IS_DOUBLE, 0)
   ZEND_END_ARG_INFO()

   ZEND_FUNCTION(fahrenheitToCelcius);
   ZEND_METHOD(Atmosphere, calculateBar);

   static const zend_function_entry ext_functions[] = {
       ZEND_FE(fahrenheitToCelcius, arginfo_fahrenheitToCelcius)
       ZEND_FE_END
   };

   static const zend_function_entry class_Atmosphere_methods[] = {
       ZEND_ME(Atmosphere, calculateBar, arginfo_class_Atmosphere_calculateBar, ZEND_ACC_PUBLIC)
       ZEND_FE_END
   };

The generated ``ext_functions`` variable must be passed as the ``functions`` member of
`zend_module_entry` struct.

The generated ``class_Atmosphere_methods`` must be used when registering the ``Atmosphere`` class:

.. code:: c

   INIT_CLASS_ENTRY(ce, "Atmosphere", class_Atmosphere_methods);

Additional meta information can be attached to functions, with the following PHPDoc tags:

-  ``@deprecated``: Triggers the usual deprecation notice when the function/method is called.

-  ``@alias``: If a function/method is an alias of another function/method, then the aliased
   function/method name has to be provided as value. E.g. the function ``sizeof()` has the ``@alias
   count`` annotation.

-  ``@implementation-alias``: This is very similar to ``@alias`` with some semantic differences.
   These aliases exists purely to avoid duplicating some code, but there is no other connection
   between the alias and the aliased function or method.

   A notable example is ``Error::getCode()``, which has the ``@implementation-alias
   Exception::getCode`` annotation.

   The difference between ``@alias`` and ``@implementation-alias`` is very nuanced and is only
   observable in the manual.

-  ``@tentative-return-type``: By using this annotation, the return type declaration is reclassified
   as a `tentative return type`_.

-  ``@genstubs-expose-comment-block``: By adding this annotation at the beginning of a PHPDoc block,
   the content of the PHPDoc block will be exposed for
   `ReflectionFunctionAbstract::getDocComment()`. This feature was added in PHP 8.4.0.

.. _tentative return type: https://wiki.php.net/rfc/internal_method_return_types

**************************
 Generating Class Entries
**************************

In order to generate code which is necessary for registering constants, classes, properties, enums,
and traits, use the ``@generate-class-entries`` file-level PHPDoc block.

``@generate-class-entries`` implies ``@generate-function-entries```, so the latter is then
superfluous.

Given the following stub:

.. code:: php

    <?php
    /** @generate-class-entries */

   enum Number: string {
       /** @var string */
       public const ONE = "one";

       case One = Number::ONE;
       case Two = Number::TWO;
   }

   class Elephant extends stdClass {
       /** @cvalue M_PI */
       public const float PI = UNKNOWN;

       public readonly string $name;
   }

The following arginfo file is generated:

.. code:: c

   static const zend_function_entry class_Number_methods[] = {
       ZEND_FE_END
   };

   static const zend_function_entry class_Elephant_methods[] = {
       ZEND_FE_END
   };

   static zend_class_entry *register_class_Number(void)
   {
       zend_class_entry *class_entry = zend_register_internal_enum("Number", IS_STRING, class_Number_methods);

       ...

       return class_entry;
   }

   static zend_class_entry *register_class_Elephant(zend_class_entry *class_entry_stdClass)
   {
       zend_class_entry ce, *class_entry;

       INIT_CLASS_ENTRY(ce, "Elephant", class_Elephant_methods);
       class_entry = zend_register_internal_class_ex(&ce, class_entry_stdClass);

       ...

       return class_entry;
   }

The generated ``register_class_*()`` functions must be used to register these classes in the
``PHP_MINIT_FUNCTION`` directly:

.. code:: c

   zend_class_entry *number_ce = register_class_Number();
   zend_class_entry *elephpant_ce = register_class_Elephant(zend_standard_class_def);

Class dependencies, such as the parent class or implemented interface, have to be passed to the
register function. In the example above, we passed the class entry for ``stdClass``
(``zend_standard_class_def``).

Like functions and methods, classes also support meta information passed via PHPDoc tags:

-  ``@deprecated``: triggers a deprecation notice when the class is used

-  ``@strict-properties``: adds the ``ZEND_ACC_NO_DYNAMIC_PROPERTIES`` flag for the class (as of PHP
   8.0), which disallow dynamic properties.

-  ``@not-serializable``: adds the ``ZEND_ACC_NOT_SERIALIZABLE`` flag for the class (as of PHP 8.1),
   which prevents the serialization of the class.

-  ``@genstubs-expose-comment-block``: By adding this tag at the beginning of a PHPDoc block, the
   content of the PHPDoc block will be exposed for `ReflectionClass::getDocComment()`. This feature
   is only available as of PHP 8.4.0.

This is an example with all the flags:

.. code:: php

   <?php
   /**
    * @generate-class-entries
    */

   /**
    * @deprecated
    * @not-serializable
    * @strict-properties */
   /** @genstubs-expose-comment-block
    * This is a comment
    * @see https://www.php.net */
   class Elephant extends stdClass {
      public readonly string $name;
   }

Resulting in these changes:

.. code:: c

   ...

   static zend_class_entry *register_class_Elephant(zend_class_entry *class_entry_stdClass)
   {
       zend_class_entry ce, *class_entry;

       INIT_CLASS_ENTRY(ce, "Elephant", class_Elephant_methods);
       class_entry = zend_register_internal_class_ex(&ce, class_entry_stdClass);
       class_entry->ce_flags |= ZEND_ACC_DEPRECATED|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
       class_entry->doc_comment = zend_string_init_interned("/**\n * This is a comment\n * @see https://www.php.net */", 55, 1);

   ...

       return class_entry;
   }

********************************************
 Generating Global Constants and Attributes
********************************************

Although global constants and function attributes do not relate to classes, they require the ``/**
@generate-class-entries */`` file-level PHPDoc block.

If a global constant or function attribute are present in the stub file, the generated C-code will
include a ``register_{$stub_file_name}_symbols()`` file.

Given the following file:

.. code:: php

   // example.stub.php
   <?php
   /** @generate-class-entries */

   /** @var string */
   const ANIMAL = "Elephant";

   /**
   * @var float
   * @cvalue M_PI
   */
   const BAR = UNKNOWN;

   function connect(#[\SensitiveParameter] string $connectionString): string {}

The following C function will be generated in order to register the two global constants and the
attribute. The name of this file is ``example.stub.php``:

.. code:: c

   ...

   static void register_example_symbols(int module_number)
   {
       REGISTER_STRING_CONSTANT("ANIMAL", "Elephant", CONST_PERSISTENT);
       REGISTER_DOUBLE_CONSTANT("BAR", M_PI, CONST_PERSISTENT);


       zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "connect", sizeof("connect") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
   }

Similarly to class registration functions, the generated ``register_{$stub_file_name}_symbols()``
functions must be used in ``PHP_MINIT_FUNCTION``, to make the global constants an attributes
available:

.. code:: c

   PHP_MINIT_FUNCTION(example)
   {
       register_example_symbols(module_number);

       return SUCCESS;
   }

Global constants always need to have their type specified with a ``@var`` PHPDoc tag. The type for
class constants is inferred from their type declaration if available, otherwise a ``@var`` PHPDoc
tag is required. A ``@var`` tag is also required if you enable ``generate-legacy-arginfo`` (see
below).

If a constant's value is defined by a 3rd party library, PHP's internals, or a specific type such as
a bitmask, the exact value is not yet known when stubs are used. In these cases, don't duplicate the
value in the stub file, but instead use the ``UNKNOWN`` constant value with the ``@cvalue`` PHPDoc
tag.

In the example below we define the ``BAR`` global constant to ``UNKNOWN``, with the value linked
with ``@cvalue M_PI`` to the C-level constant ``M_PI`` (define by PHP's internals).

Constants can take the following extra meta information passed via PHPDoc tags:

-  ``@deprecated``: Triggers a deprecation notice when the constant is used.

-  ``@genstubs-expose-comment-block``: By adding this tag at the beginning of a PHPDoc block, the
   content of the PHPDoc block will be exposed for `ReflectionClass::getDocComment()`. This feature
   is only available as of PHP 8.4.0.

************************************
 Maintaining Backward Compatibility
************************************

The stubs in the PHP source distribution only need to support the branch they are part of.

Third party extensions often need to support a wider range of PHP versions, with different features
supported, that can be enabled through stubs.

Stubs may get new features which are unavailable in earlier PHP versions, or ABI compatibility
breaks may happen between minor releases. And PHP 7.x versions are substantially different from PHP
8 versions.

It is possible to tell the arginfo generator script ``gen_stub.php`` to create legacy arginfo too,
specifying a minimum supported version.

If your extension still needs to handle PHP 7, then add the ``@generate-legacy-arginfo`` file-level
PHPDoc tag, without any value. In this case, an additional ``_legacy_arginfo.h`` file will be
generated. You can include this file conditionally, such as:

.. code::

   #if (PHP_VERSION_ID >= 80000)
   # include "example_arginfo.h"
   #else
   # include "example_legacy_arginfo.h"
   #endif

When ``@generate-legacy-arginfo`` is passed the minimum PHP version ID that needs to be supported,
then only one arginfo file is going to be generated, and ``#if`` prepocessor directives will ensure
compatibility with all the required PHP 8 versions.

PHP Version IDs are as follows: ``80000`` for PHP 8.0, ``80100`` for PHP PHP 8.1, ``80200`` for PHP
8.2, ``80300`` for PHP 8.3, and ``80400`` for PHP 8.4,

In this example we add a PHP 8.0 compatibility requirement to a slightly modified version of a
previous example:

.. code:: php

   <?php
   /**
    * @generate-class-entries
    * @generate-legacy-arginfo 80000
    */

   enum Number: string {
      case One;
   }

   /**
    * @strict-properties
    * @not-serializable */
   class Elephant {
      /**
       * @cvalue M_PI
       * @var float
       */
      public const float PI = UNKNOWN;

      public readonly string $name;
   }

Then notice the ``#if (PHP_VERSION_ID >= ...)`` conditions in the generated arginfo file:

.. code:: c

   ...

   #if (PHP_VERSION_ID >= 80100)
   static zend_class_entry *register_class_Number(void)
   {
       zend_class_entry *class_entry = zend_register_internal_enum("Number", IS_STRING, class_Number_methods);

       zend_enum_add_case_cstr(class_entry, "One", NULL);

       return class_entry;
   }
   #endif

   static zend_class_entry *register_class_Elephant(void)
   {
       zend_class_entry ce, *class_entry;

       INIT_CLASS_ENTRY(ce, "Elephant", class_Elephant_methods);
       class_entry = zend_register_internal_class_ex(&ce, NULL);
   #if (PHP_VERSION_ID >= 80100)
       class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
   #elif (PHP_VERSION_ID >= 80000)
       class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
   #endif

       zval const_PI_value;
       ZVAL_DOUBLE(&const_PI_value, M_PI);
       zend_string *const_PI_name = zend_string_init_interned("PI", sizeof("PI") - 1, 1);
   #if (PHP_VERSION_ID >= 80300)
       zend_declare_typed_class_constant(class_entry, const_PI_name, &const_PI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
   #else
       zend_declare_class_constant_ex(class_entry, const_PI_name, &const_PI_value, ZEND_ACC_PUBLIC, NULL);
   #endif
       zend_string_release(const_PI_name);

       zval property_name_default_value;
       ZVAL_UNDEF(&property_name_default_value);
       zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
   #if (PHP_VERSION_ID >= 80100)
       zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
   #elif (PHP_VERSION_ID >= 80000)
       zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
   #endif
       zend_string_release(property_name_name);

       return class_entry;
   }

The preprocessor conditions are necessary because ``enum``s, ``readonly`` properties, and the
``not-serializable`` flag, are PHP 8.1 features and don't exist in PHP 8.0.

The registration of ``Number`` is therefore completely omitted, while the ``readonly`` flag is not
added for``Elephpant::$name`` for PHP versions before 8.1.

Additionally, typed class constants are new in PHP 8.3, and hence a different registration function
is used for versions before 8.3.

******************************************
 Generating Information for the Optimizer
******************************************

A list of functions is maintained for the optimizer in ``Zend/Optimizer/zend_func_infos.h``. This
file contains extra information about the return type and the cardinality of the return value. This
can enable more accurate optimizations (i.e. better type inference).

Previously, the file was maintained manually, but since PHP 8.1, ``gen_stub.php`` can take care of
this with the ``--generate-optimizer-info`` option.

This feature is only available for built-in stubs inside php-src, since currently there is no way to
provide the function list for the optimizer other than overwriting ``zend_func_infos.h`` directly.

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

Functions can be evaluated at compile-time if their arguments are known in compile-time, and their
behavior is free from side-effects and is not affected by the global state.

The list of such functions in the optimizer was maintained manually until PHP 8.2.

Since PHP 8.2, the ``@compile-time-eval`` PHPDoc tag can be applied to any function which conforms
to the above restrictions in order for them to qualify as evaluable at compile-time. The feature
internally works by adding the ``ZEND_ACC_COMPILE_TIME_EVAL`` function flag.

In PHP 8.4, arity-based frameless functions were introduced. This is another optimization technique,
which results in faster internal function calls by eliminating unnecessary checks for the number of
passed parameters—if the number of passed arguments is known at compile-time.

To take advantage of frameless functions, add the ``@frameless-function`` PHPDoc tag with some
configuration.

Since only arity-based optimizations are supported, the tag has the form: ``@frameless-function
{"arity": NUM}``. ``NUM`` is the number of parameters for which a frameless function is available.

The stub of ``in_array()`` is a good example:

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
 Generating Signatures for the Manual
**************************************

The manual should reflect the exact same signatures which are represented by the stubs. This is not
exactly the case yet for built-in symbols, but ``gen_stub.php`` has multiple features to automate
the process of synchronization.

Newly added functions or methods can be documented by providing the ``--generate-methodsynopses``
option.

Running ``./build/gen_stub.php --generate-methodsynopses ./ext/mbstring
../doc-en/reference/mbstring`` will create a dedicated page for each ``ext/mbstring`` function which
is not yet documented, and saves them into the ``../doc-en/reference/mbstring/functions`` directory.

Since these are stub documentation pages, many of the sections are empty. Relevant descriptions have
to be added, and irrelevant sections should be removed.

Functions or methods that are already available in the manual, the documented signatures can be
updated by providing the ``--replace-methodsynopses`` option.

Running ``./build/gen_stub.php --replace-methodsynopses ./ ../doc-en/`` will update the function or
method signatures in the English documentation whose stub counterpart is found.

Class signatures can be updated in the manual by providing the ``--replace-classsynopses`` option.

Running ``./build/gen_stub.php --replace-classsynopses ./ ../doc-en/`` will update all the class
signatures in the English documentation whose stub counterpart is found.

If a symbol is not intended to be documented, the ``@undocumentable`` PHPDoc tag should be added to
it. Doing so will prevent any documentation to be created for the given symbol. To avoid a whole
stub file to be added to the manual, this PHPDoc tag should be applied to the file itself.

These flags are useful for symbols which exist only for testing purposes (e.g. the ones declared for
``ext/zend_test``), or by some other reason documentation is not possible.

************
 Validation
************

You can use the ``--verify`` flag to ``gen_stub.php`` to validate whether the alias function/method
signatures are correct.

An alias function/method should have the exact same signature as its aliased function/method
counterpart, apart from the name. In some cases this is not possible. For example. ``bzwrite()`` is
an alias of ``fwrite()``, but the name of the first parameter is different because the resource
types differ.

In order to suppress the error when the check is false positive, the ``@no-verify`` PHPDoc tag
should be applied to the alias:

.. code:: php

   /**
    * @param resource $bz
    * @implementation-alias fwrite
    * @no-verify Uses different parameter name
    */
   function bzwrite($bz, string $data, ?int $length = null): int|false {}

Besides aliases, the contents of the documentation can also be validated by providing the
``--verify-manual`` option to ``gen_stub.php``. This flag requires the directory with the source
stubs, and the target manual directory, as in ``./build/gen_stub.php --verify-manual ./
../doc-en/``.

For this validation, all ``php-src`` stubs and the full English documentation should be available by
the specified path.

This feature performs the following validations:

-  Detecting missing global constants
-  Detecting missing classes
-  Detecting missing methods
-  Detecting incorrectly documented alias functions or methods

Running it with the stub examples that are used in this guide, the following warnings are shown:

.. code:: shell

   Warning: Missing class synopsis for Number
   Warning: Missing class synopsis for Elephant
   Warning: Missing class synopsis for Atmosphere
   Warning: Missing method synopsis for fahrenheitToCelcius()
   Warning: Missing method synopsis for Atmosphere::calculateBar()

**********************
 Parameter Statistics
**********************

The ``gen_stub.php`` flag ``--parameter-stats`` counts how many times a parameter name occurs in the
codebase.

A JSON object is displayed, containing the parameter names and the number of their occurrences in
descending order.
