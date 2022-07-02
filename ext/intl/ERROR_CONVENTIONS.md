# Intl extension error conventions

The intl extension has particular conventions regarding error reporting. These
conventions are enumerated in this document.

* The last error is always stored globally.

The global error code can be obtained in userland with `intl_get_error_code()`.
This is a `U_*` error code defined by ICU, but it does not have necessarily to
be returned obtained after a call to an ICU function. That is to say, the
internal PHP wrapper functions can set these error codes when appropriate. For
instance, in response to bad arguments (e.g. `zend_parse_parameters()` failure),
the PHP wrapper function should set the global error code to
`U_ILLEGAL_ARGUMENT_ERROR`).

The error code (an integer) can be converter to the corresponding enum name
string in userland with `intl_error_name()`.

The associated message can be obtained with `intl_get_error_message()`. This is
a message set by the PHP wrapping code, not by ICU. The message should include
the name of the function that failed in order to make debugging easier (though
if you activate warnings with `intl.error_level` or exceptions with
`intl.use_exceptions` you get more fine-grained information about where the
error occurred).

The internal PHP code can set the global last error with:

```c
void intl_error_set_code(intl_error* err, UErrorCode err_code);
void intl_error_set_custom_msg(intl_error* err, char* msg, int copyMsg);
void intl_error_set(intl_error* err, UErrorCode code, char* msg, int copyMsg);
```

and by passing `NULL` as the first parameter. The last function is a combination
of the first two. If the message is not a static buffer, `copyMsg` should be 1.
This makes the message string be copied and freed when no longer needed. There's
no way to pass ownership of the string without it being copied.

* The last is ALSO stored in the object whose method call triggered the error,
  unless the error is due to bad arguments, in which case only the global error
  should be set.

Objects store an intl_error structed in their private data. For instance:

```c
typedef struct {
    zend_object zo;
    intl_error  err;
    Calendar*   ucal;
} Calendar_object;
```

The global error and the object error can be SIMULTANEOUSLY set with these
functions:

```c
void intl_errors_set_custom_msg(intl_error* err, char* msg, int copyMsg);
void intl_errors_set_code(intl_error* err, UErrorCode err_code);
void intl_errors_set(intl_error* err, UErrorCode code, char* msg, int copyMsg);
```

by passing a pointer to the object's `intl_error` structed as the first parameter.
Node the extra `s` in the functions' names (`errors`, not `error`).

Static methods should only set the global error.

* Intl classes that can be instantiated should provide `::getErrorCode()` and
  `getErrorMessage()` methods.

These methods are used to retrieve the error codes stored in the object's
private `intl_error` structured and mirror the global `intl_get_error_code()`
and `intl_get_error_message()`.

* Intl methods and functions should return `FALSE` on error (even argument
  parsing errors), not `NULL`. Constructors and factory methods are the
  exception; these should return `NULL`, not `FALSE`.

Not that constructors in Intl generally (always?) don't throws exceptions. They
instead destroy the object to that the result of new `IntlClass()` can be
`NULL`. This may be surprising.

* Intl functions and methods should reset the global error before doing anything
  else (even parse the arguments); instance methods should also reset the
  object's private error.

Errors should be lost after a function call. This is different from the way ICU
operates, where functions return immediately if an error is set.

Error resetting can be done with:

```c
void intl_error_reset(NULL);             /* reset global error */
void intl_errors_reset(intl_error* err); /* reset global and object error */
```

In practice, `intl_errors_reset()` is not used because most classes have also
plain functions mapped to the same internal functions as their instance methods.
Fetching of the object is done with `zend_parse_method_parameters()` instead of
directly using `getThis()`. Therefore, no reference to object is obtained until
the arguments are fully parsed. Without a reference to the object, there's no
way to reset the object's internal error code. Instead, resetting of the
object's internal error code is done upon fetching the object from its zval.

Example:

```c
U_CFUNC PHP_FUNCTION(breakiter_set_text)
{
    /* ... variable declarations ... */
    BREAKITER_METHOD_INIT_VARS; /* macro also resets global error */
    object = getThis();

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
            &text, &text_len) == FAILURE) {
        intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
            "breakiter_set_text: bad arguments", 0);
        RETURN_THROWS();
    }

    /* ... */

    BREAKITER_METHOD_FETCH_OBJECT; /* macro also resets object's error */

    /* ... */
}
```

Implementations of `::getErrorCode()` and `::getErrorMessage()` should not reset
the object's error code.
