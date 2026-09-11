#######################################################
 PHP Callables (zend_fcall_info/zend_fcall_info_cache)
#######################################################

A common pattern for custom behaviour is to accept a user callback. As such we need a way to
represent PHP callables in C. For this purpose there exists two structures ``zend_fcall_info`` and
``zend_fcall_info_cache``. ``zend_fcall_info_cache`` is a pure representation of a PHP callable, and
should be the structure used to store PHP callables. ``zend_fcall_info`` contains all the required
information to *perform* a call to a PHP callable. We will use the commonly used abbreviation of FCI
and FCC when talking about ``zend_fcall_info`` and ``zend_fcall_info_cache`` respectively. The most
likely way to encounter those structs is by using the ZPP ``f`` argument specifier.

**********************
 Generic callable API
**********************

There is a set of common parameters used within the callable APIs which are:

.. list-table:: Common callable API parameter meaning
   :header-rows: 1

   -  -  Name
      -  Type
      -  Description

   -  -  ``fn``
      -  ``zend_function*``
      -  The function pointer to call, usually stored in ``EG(function_table)`` for global functions
         or ``zend_class_entry->function_table`` for class methods.

   -  -  ``object``
      -  ``zend_object*``
      -  Generally object pointer representing ``$this``.

   -  -  ``called_scope``
      -  ``zend_class_entry*``
      -  Scope where the method is called, ``NULL`` for global functions.

   -  -  ``retval``
      -  ``zval*``
      -  zval pointer where the return value of the called function will be stored. Usually
         mandatory.

   -  -  ``param_count``
      -  ``uint32_t``
      -  Number of parameters passed to the function via the ``params`` argument.

   -  -  ``params``
      -  ``zval*``
      -  Pointer to a C zval array containing the arguments to the function, may be ``NULL`` if
         ``param_count`` is 0.

   -  -  ``named_params``

      -  ``HashTable*``

      -  HashTable containing named argument as name=>val, can also contain positional argument if
         the key is numeric (in which case the positional argument is pushed as the next argument
         regardless of its index)

There are some functions that do not require the use of either the FCI or the FCC structs:

.. list-table:: Basic callable API
   :width: 100%
   :widths: 30 70
   :header-rows: 1

   -  -  Function/Macro
      -  Description

   -  -  .. code:: c

            zend_is_callable(
                zval *callable,
                uint32_t check_flags,
                zend_string **callable_name
            )

      -  Check if the ``callable`` zval is a PHP callable. Returns true if it is, false otherwise.
         ``check_flags`` is a bit mask of ``IS_CALLABLE_CHECK_SYNTAX_ONLY`` and
         ``IS_CALLABLE_SUPPRESS_DEPRECATIONS``, generally should be left to 0. ``callable_name`` is
         an optional out parameter, it will be set to the string representation of the ``callable``
         zval, set even in case of failure.

   -  -  .. code:: c

            zend_is_callable_ex(
                zval *callable,
                zend_object *object,
                uint32_t check_flags,
                zend_string **callable_name,
                zend_fcall_info_cache *fcc,
                char **error
            )

      -  Same as ``zend_is_callable()`` with additional arguments. ``object`` if ``callable`` is a
         string, this ``zend_object*`` represents the class instance to check if such a method
         exists. ``fcc`` is an out parameter constructing the FCC struct, see below for details.
         ``error`` is an optional out parameter holding the error message if ``callable`` is not a
         PHP callable.

   -  -  .. code:: c

            zend_call_known_function(
                zend_function *fn,
                zend_object *object,
                zend_class_entry *called_scope,
                zval *retval,
                uint32_t param_count,
                zval *params,
                HashTable *named_params
            )

      -  Call a known function with given parameters. ``retval`` may be ``NULL`` in which case the
         return value of the function is discarded. If ``object`` is ``NULL`` it must be a global
         function or static method. ``called_scope`` must *not* be ``NULL`` if ``fn`` is a method
         (instanced or static).

   -  -  .. code:: c

            zend_call_known_function_ex(
                zend_function *fn,
                zend_object *object,
                zend_class_entry *called_scope,
                zval *retval,
                uint32_t param_count,
                zval *params,
                HashTable *named_params
                uint32_t consumed_args
            )

      -  Same as ``zend_call_known_function()``. TODO explain consumed_args uint32_t arg.

   -  -  .. code:: c

            zend_call_known_instance_method(
                zend_function *fn,
                zend_object *object,
                zval *retval,
                uint32_t param_count,
                zval *params
            )

      -  Same as ``zend_call_known_function()``, where the ``called_scope`` is derived from
         ``object->ce``.

   -  -  .. code:: c

            zend_call_known_instance_method_with_0_params(
                zend_function *fn,
                zend_object *object,
                zval *retval,
            )

      -  Helper function around ``zend_call_known_instance_method()``.

   -  -  .. code:: c

            zend_call_known_instance_method_with_1_params(
                zend_function *fn,
                zend_object *object,
                zval *retval,
                zval *param
            )

      -  Helper function around ``zend_call_known_instance_method()``.

   -  -  .. code:: c

            zend_call_known_instance_method_with_2_params(
                zend_function *fn,
                zend_object *object,
                zval *retval,
                zval *param1,
                zval *param2
            )

      -  Helper function around ``zend_call_known_instance_method()``.

   -  -  .. code:: c

            zend_call_method_if_exists(
                zend_object *object,
                zend_string *method_name,
                zval *retval,
                uint32_t param_count,
                zval *params
            )

      -  Returns ``SUCCESS`` if the ``method_name`` exists on the object and the call succeeds,
         ``FAILURE`` otherwise. If ``FAILURE`` is returned the retval *will* be UNDEF, as such the
         retval can be unconditionally destroyed.

   -  -  .. code:: c

            call_user_function(
                function_table,
                object,
                function_name,
                retval,
                param_count,
                params
            )

      -  Deprecated API. Checks that ``function_name`` is a valid PHP callable via
         ``zend_is_callable_ex()`` and calls it with the given parameters and return value.
         ``function_table`` is ignored and should always be ``NULL``. It is recommended to instead
         use ``zend_is_callable_ex()`` to fetch an FCC and call it with ``zend_call_known_fcc()``,
         see below for more details.

   -  -  .. code:: c

            call_user_function_named(
                function_table,
                object,
                function_name,
                retval,
                param_count,
                params,
                named_params
            )

      -  Deprecated API. Same as ``call_user_function()`` with the extra ``named_params`` argument.

***********************
 zend_fcall_info_cache
***********************

.. code:: c

   typedef struct _zend_fcall_info_cache {
       zend_function *function_handler;
       zend_class_entry *calling_scope;
       zend_class_entry *called_scope;
       zend_object *object; /* Object representing $this, only needed for instanced methods */
       zend_object *closure; /* Closure reference, only if the callable *is* the object */
   } zend_fcall_info_cache;

``function_handler``:
   The actual body of a PHP function that will be used by the VM, can be retrieved from the global
   function table or a class function table (``zend_class_entry->function_table``). May be ``NULL``
   for trampolines, see details below.

``object``:
   If the function is an instance object method, this field represents the value of `$this`. Should
   be ``NULL`` for static methods and global functions.

``called_scope``:
   The scope in which to call the method, generally it's ``object->ce``.

``calling_scope``:
   The scope in which this call is made, only used by ``zend_is_callable_ex()`` to resolve the
   actual scope for the deprecated partially supported callables (e.g. ``[$obj,
   "parent::method"]``). Can be ignored as it is not used when actually calling a function.

``closure``:
   Storage field for the ``Closure`` object or the object containing the ``__invoke()`` /
   ``__call()`` / ``__callStatic()`` method pointed at in ``function_handler``.

.. note::

   Prior to PHP 8.3.0, the FCC did not contain the ``closure`` field, and the relevant FCC API
   didn't exist.

FCCs are the goto structure to handle and store PHP callables as most of the time they don't need
any reference counting. The one exception is the FCC represents a trampoline. A trampoline is a call
to a non existing class method handled by the ``__call()`` or ``__callStatic()`` magic methods. When
a trampoline is created it allocates a ``zend_function`` struct with the op array copied, and freed
when called. However the ``f`` ZPP argument specifier will *free* the trampoline and assign ``NULL``
to ``function_handler``, making the FCC uninitialized. The trampoline can be manually fetched by
using ``zend_is_callable_ex()`` or to prevent ZPP from freeing it one can use the ``F`` argument
specifier`. However, in that case careful consideration need to be done to free the potential
trampoline in any failure path, including ZPP failure.

The *only* case where an FCC will be uninitialized is if the function is a trampoline, i.e. when the
method of a class does not exist but is handled by the magic methods
``__call()``/``__callStatic()``. This is because a trampoline is freed by ZPP as it is a newly
allocated ``zend_function`` struct with the op array copied, and is freed when called. To retrieve
it manually use ``zend_is_callable_ex()``. To free a trampoline the ``void
zend_release_fcall_info_cache(zend_fcall_info_cache *fcc)`` should be used to release it. This
function can be called even if the FCC is not a trampoline. However, if a reference to the closure
is kept, this must be called *prior* to freeing the ``closure``, as the trampoline will partially
refer to a ``zend_function *`` entry in the closure CE.

.. note::

   There is investigative work being done to simplify the handling of trampoline in the future.

FCC API
=======

The FCC API is defined in ``Zend/zend_API.h``.

.. list-table:: ``zend_fcall_info_cache`` API
   :width: 100%
   :widths: 30 70
   :header-rows: 1

   -  -  Function/Macro
      -  Description

   -  -  ``ZEND_FCC_INITIALIZED(fcc)``
      -  Check if an FCC is initialized.

   -  -  .. code:: c

            zend_fcc_addref(
                zend_fcall_info_cache *fcc
            )

      -  Adds necessary reference increments and copies to an FCC pointer.

   -  -  .. code:: c

            zend_fcc_dup(
                zend_fcall_info_cache *dest,
                const zend_fcall_info_cache *src,
            )

      -  Duplicates the source FCC pointer into the destination FCC pointer.

   -  -  ``zend_fcc_equals(fcc_1, fcc_2)``
      -  Determines if two FCCs represent the same function.

   -  -  ``zend_fcc_closure_equals_ex(fcc_1, fcc_2)``
      -  Determine if both FCCs represent the same function as a ``Closure`` may be a
         ``ZEND_ACC_FAKE_CLOSURE``

   -  -  .. code:: c

            zend_fcc_dtor(
                zend_fcall_info_cache *fcc
            )

      -  Releases data hold by an FCC, including freeing trampolines.

   -  -  ``zend_get_gc_buffer_add_fcc(gc_buffer, fcc)``
      -  For use with the ``get_gc`` object handler in case the FCC is stored by an internal object.

   -  -  .. code:: c

            zend_call_known_fcc(
                zend_fcall_info_cache *fcc,
                zval *retval,
                uint32_t param_count,
                zval *params,
                HashTable *named_params
            )

      -  Calls an initialized FCC, all other parameters are optional.

   -  -  ``zend_get_callable_zval_from_fcc(fcc, callable)``
      -  Creates a representation of the FCC into the given callable `zval*`. Note static class
         methods that can be represented as strings or an array pair are returned as an array pair

   -  -  .. code:: c

            zend_release_fcall_info_cache(
                zend_fcall_info_cache *fcc
            )

      -  Release potential trampoline held by the FCC.

*****************
 zend_fcall_info
*****************

.. code:: c

   typedef struct _zend_fcall_info {
       size_t size;
       zval function_name;
       zval *retval;
       zval *params;
       zend_object *object;
       uint32_t param_count;
       uint32_t consumed_args;
       /* This hashtable can also contain positional arguments (with integer keys),
        * which will be appended to the normal params[]. This makes it easier to
        * integrate APIs like call_user_func_array(). The usual restriction that
        * there may not be position arguments after named arguments applies. */
       HashTable *named_params;
   } zend_fcall_info;

``size``:
   Mandatory field, which is the size of an FCI structure, thus always: ``sizeof(zend_fcall_info)``

``function_name``:
   The actual callable, do not be fooled by the name of this field as this is a leftover when PHP
   didn't have objects and class methods. It must be a string zval or an array following the same
   rules as callables in PHP, namely the first index is a class or instance object, and the second
   one is the method name. If an initialized FCC representing the function exists, it may be
   omitted.

``object``:
   Only used for callability checks in conjunction with ``function_name``, see
   ``zend_is_callable_ex()`` for more details. If an initialized FCC representing the function
   exists, it may be omitted.

``retval``:
   Mandatory field, which will contain the result of the PHP function.

``param_count``:
   Mandatory field, the number of arguments that will be provided to this call to the function.

``consumed_args``:
   TBD new since PHP 8.6. and needs docs

``params``:
   contains positional arguments that will be provided to this call to the function. If
   ``param_count = 0``, it can be ``NULL``.

``named_params``:
   A HashTable containing named (or positional) arguments.

FCI API
=======

The FCI API is defined in ``Zend/zend_API.h``.

.. list-table:: ``zend_fcall_info`` API
   :width: 100%
   :widths: 10 90
   :header-rows: 1

   -  -  Function/Macro
      -  Description

   -  -  ``ZEND_FCI_INITIALIZED(fci)``
      -  Check if an FCI is initialized.

   -  -  ``zend_call_function(fci, fcc)``

      -  Calls a function, ``fcc`` is a pointer to an FCC that may be ``NULL``, in this case the FCC
         will be derived from the FCI `function_name` zval, including a callability check. This
         function only returns ``FAILURE`` if ``EG(active)`` is false during late engine shutdown.
         If the call or call setup throws, EG(exception) will be set and the retval will be UNDEF.
         Otherwise, the retval will be a non-UNDEF value.

   -  -  .. code:: c

            zend_call_function_with_return_value(
                zend_fcall_info *fci,
                zend_fcall_info_cache *fcc,
                zval *retval
            )

      -  Same as ``zend_call_function`` but sets the FCI return value to `retval`.

   -  -  .. code:: c

            zend_fcall_info_init(
                const zval *callable,
                uint32_t check_flags,
                zend_fcall_info *fci,
                zend_fcall_info_cache *fcc,
                zend_string **callable_name,
                char **error
            )

      -  Build a FCI/FCC pair from a ``zval*`` by forwarding ``callable``, ``check_flags``, ``fcc``,
         ``callable_name``, and ``error`` to ``zend_is_callable_ex()``. As such ``callable_name``
         and ``error`` are optional and may be ``NULL``. Returns ``FAILURE`` if ``callable`` is not
         a PHP callable, ``SUCCESS`` otherwise.

Deprecated FCI API
==================

There is also a set of FCI API that is now considered deprecated as it is unintuitive and does
unnecessary parameter copying.

In general the IS_ARRAY zval should be assigned to the ``named_params`` field of the FCI instead of
copying the arguments.

.. list-table:: Deprecated ``zend_fcall_info`` API
   :width: 100%
   :widths: 30 70
   :header-rows: 1

   -  -  Function
      -  Description

   -  -  ``zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval, zval
         *args)``

      -  Call a function using information created by zend_fcall_info_init()/args() functions. If
         args is given then those replace the argument info in FCI is temporarily. ``args`` *must*
         be an IS_ARRAY zval. It is recommended to replace calls to this function by
         ``zend_call_function_with_return_value()`` and if ``args`` is not ``NULL`` to set the
         ``fci.named_params = Z_ARR_P(args);``.

   -  -  ``zend_fcall_info_args_clear(zend_fcall_info *fci, bool free_mem)``
      -  Clear arguments connected with the FCI. If free_mem is true then the params array gets
         free'd as well.

   -  -  ``zend_fcall_info_args_save(zend_fcall_info *fci, uint32_t *param_count, zval **params)``
      -  Save current arguments from the FCI, the params array will be set to ``NULL``.

   -  -  ``zend_fcall_info_args_restore(zend_fcall_info *fci, uint32_t param_count, zval *params)``
      -  Free arguments connected with the FCI, and set back saved ones.

   -  -  ``zend_fcall_info_args(zend_fcall_info *fci, zval *args)``
      -  Sets the arguments in the FCI taking care of refcount. ``args`` *must* be an IS_ARRAY zval.
         If ``args`` is ``NULL`` the FCI arguments are cleared.

   -  -  ``zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args)``

      -  Same as ``zend_fcall_info_args()`` but if ``func`` is not ``NULL`` it will check if a
         by-value argument needs to be passed by-ref and wraps it in a reference. This behaviour is
         **NON STANDARD** PHP behaviour and should never be used.

   -  -  ``zend_fcall_info_argp(zend_fcall_info *fci, uint32_t argc, zval *argv)``
      -  Sets the arguments in the FCI taking care of refcount. If ``argc`` is ``0`` the FCI
         arguments are cleared, else pass a variable amount of zval** arguments.

   -  -  ``zend_fcall_info_argv(zend_fcall_info *fci, uint32_t argc, va_list *argv)``
      -  Sets the arguments in the FCI taking care of refcount. If ``argc`` is ``0`` the FCI
         arguments are cleared, else pass a variable amount of zval** arguments.

   -  -  ``zend_fcall_info_argn(zend_fcall_info *fci, uint32_t argc, ...)``
      -  Sets the arguments in the FCI taking care of refcount. If ``argc`` is ``0`` the FCI
         arguments are cleared, else pass a variable amount of zval** arguments.
