# Fast Parameter Parsing API

In PHP 7, a "Fast Parameter Parsing API" was introduced. See
[RFC](https://wiki.php.net/rfc/fast_zpp).

This API uses inlining to improve applications performance compared with the
`zend_parse_parameters()` function described below.

## Parameter parsing functions

Borrowing from Python's example, there is a set of functions that given the
string of type specifiers, can parse the input parameters and store the results
in the user specified variables. This avoids using `IS_*` checks and
`convert_to_*` conversions. The functions also check for the appropriate number
of parameters, and try to output meaningful error messages.

## Prototypes

```c
/* Implemented. */
int zend_parse_parameters(int num_args, char *type_spec, ...);
int zend_parse_parameters_ex(int flags, int num_args, char *type_spec, ...);
```

The `zend_parse_parameters()` function takes the number of parameters passed to
the extension function, the type specifier string, and the list of pointers to
variables to store the results in. The _ex() version also takes 'flags' argument
-- current only `ZEND_PARSE_PARAMS_QUIET` can be used as 'flags' to specify that
the function should operate quietly and not output any error messages.

Both functions return `SUCCESS` or `FAILURE` depending on the result.

The auto-conversions are performed as necessary. Arrays, objects, and resources
cannot be auto-converted.

PHP 5.3 includes a new function (actually implemented as macro):

```c
int zend_parse_parameters_none();
```

This returns `SUCCESS` if no argument has been passed to the function, `FAILURE`
otherwise.

PHP 5.5 includes a new function:

```c
int zend_parse_parameter(int flags, int arg_num, zval **arg, const char *spec, ...);
```

This function behaves like `zend_parse_parameters_ex()` except that instead of
reading the arguments from the stack, it receives a single zval to convert
(passed with double indirection). The passed zval may be changed in place as
part of the conversion process.

See also
[Expose zend_parse_arg() as zend_parse_parameter()](https://wiki.php.net/rfc/zpp_improv#expose_zend_parse_arg_as_zend_parse_parameter).

## Type specifiers

The following list shows the type specifier, its meaning, and the parameter types
that need to be passed by address. All passed parameters are set if the PHP
parameter is non-optional and untouched if optional and the parameter is not
present. The only exception is O where the zend_class_entry* has to be provided
on input and is used to verify the PHP parameter is an instance of that class.

```txt
a  - array (zval*)
A  - array or object (zval*)
b  - boolean (bool)
C  - class (zend_class_entry*)
d  - double (double)
f  - PHP callable containing php function/method call info (returned as
     zend_fcall_info and zend_fcall_info_cache).
     The FCC may be uninitialized if the callable is a trampoline.
F  - PHP callable containing php function/method call info (returned as
     zend_fcall_info and zend_fcall_info_cache).
     The FCC will *always* be initialized, even if the callable is a trampoline.
     A trampoline *must* be consumed or released with
     zend_release_fcall_info_cache().
h  - array (returned as HashTable*)
H  - array or HASH_OF(object) (returned as HashTable*)
l  - long (zend_long)
n  - long or double (zval*)
o  - object of any type (zval*)
O  - object of specific type given by class entry (zval*, zend_class_entry)
p  - valid path (string without null bytes in the middle) and its length (char*, size_t)
P  - valid path (string without null bytes in the middle) as zend_string (zend_string*)
r  - resource (zval*)
s  - string (with possible null bytes) and its length (char*, size_t)
S  - string (with possible null bytes) as zend_string (zend_string*)
z  - the actual zval (zval*)
*  - variable arguments list (0 or more)
+  - variable arguments list (1 or more)
```

The following characters also have a meaning in the specifier string:

* `|` - indicates that the remaining parameters are optional, they should be
  initialized to default values by the extension since they will not be touched
  by the parsing function if they are not passed to it.
* `/` - use SEPARATE_ZVAL() on the parameter it follows
* `!` - the parameter it follows can be of specified type or NULL. If NULL is
  passed, and the output for such type is a pointer, then the output pointer is
  set to a native NULL pointer. For 'b', 'l' and 'd', an extra argument of type
  bool* must be passed after the corresponding bool*, zend_long* or
  double* arguments, respectively. A non-zero value will be written to the
  bool if a PHP NULL is passed.
  For `f` use the ``ZEND_FCI_INITIALIZED(fci)`` macro to check if a callable
  has been provided and ``!ZEND_FCI_INITIALIZED(fci)`` to check if a PHP NULL
  is passed.

## Note on 64bit compatibility

Please note that since version 7 PHP uses `zend_long` as integer type and
`zend_string` with `size_t` as length, so make sure you pass `zend_long`s to "l"
and `size_t` to strings length (i.e. for "s" you need to pass char `*` and
`size_t`), not the other way round!

Both mistakes might cause memory corruptions and segfaults:

* 1

```c
char *str;
long str_len; /* XXX THIS IS WRONG!! Use size_t instead. */
zend_parse_parameters(ZEND_NUM_ARGS(), "s", &str, &str_len)
```

* 2

```c
int num; /* XXX THIS IS WRONG!! Use zend_long instead. */
zend_parse_parameters(ZEND_NUM_ARGS(), "l", &num)
```

If you're in doubt, use check_parameters.php script to the parameters and their
types (it can be found in `./scripts/dev/` directory of PHP sources):

```bash
php ./scripts/dev/check_parameters.php /path/to/your/sources/
```

## Examples

```c
/* Gets a long, a string and its length, and a zval */
zend_long l;
char *s;
size_t s_len;
zval *param;
if (zend_parse_parameters(ZEND_NUM_ARGS(), "lsz",
                          &l, &s, &s_len, &param) == FAILURE) {
    return;
}

/* Gets an object of class specified by my_ce, and an optional double. */
zval *obj;
double d = 0.5;
zend_class_entry *my_ce;
if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|d",
                          &obj, my_ce, &d) == FAILURE) {
    return;
}

/* Gets an object or null, and an array.
   If null is passed for object, obj will be set to NULL. */
zval *obj;
zval *arr;
if (zend_parse_parameters(ZEND_NUM_ARGS(), "o!a",
                          &obj, &arr) == FAILURE) {
    return;
}

/* Gets a separated array which can also be null. */
zval *arr;
if (zend_parse_parameters(ZEND_NUM_ARGS(), "a/!",
                          &arr) == FAILURE) {
    return;
}

/* Get either a set of 3 longs or a string. */
zend_long l1, l2, l3;
char *s;
/*
 * The function expects a pointer to a size_t in this case, not a long
 * or any other type. If you specify a type which is larger
 * than a 'size_t', the upper bits might not be initialized
 * properly, leading to random crashes on platforms like
 * Tru64 or Linux/Alpha.
 */
size_t length;

if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(),
                             "lll", &l1, &l2, &l3) == SUCCESS) {
    /* manipulate longs */
} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(),
                                    "s", &s, &length) == SUCCESS) {
    /* manipulate string */
} else {
    /* output error */

    return;
}

/* Function that accepts only varargs (0 or more) */

int i, num_varargs;
zval *varargs = NULL;

if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &varargs, &num_varargs) == FAILURE) {
    return;
}

for (i = 0; i < num_varargs; i++) {
    /* do something with varargs[i] */
}

if (varargs) {
    efree(varargs);
}

/* Function that accepts a string, followed by varargs (1 or more) */

char *str;
size_t str_len;
int i, num_varargs;
zval *varargs = NULL;

if (zend_parse_parameters(ZEND_NUM_ARGS(), "s+", &str, &str_len, &varargs, &num_varargs) == FAILURE) {
    return;
}

for (i = 0; i < num_varargs; i++) {
    /* do something with varargs[i] */
}

/* Function that takes an array, followed by varargs, and ending with a long */
zend_long num;
zval *array;
int i, num_varargs;
zval *varargs = NULL;

if (zend_parse_parameters(ZEND_NUM_ARGS(), "a*l", &array, &varargs, &num_varargs, &num) == FAILURE) {
    return;
}

for (i = 0; i < num_varargs; i++) {
    /* do something with varargs[i] */
}

/* Function that doesn't accept any arguments */
if (zend_parse_parameters_none() == FAILURE) {
    return;
}
```
