# PHP coding standards

This file lists standards that any programmer adding or changing code in
PHP should follow. The code base does not yet fully follow it, but new
features are going in that general direction. Many sections have been
rewritten to comply with these rules.

## Code implementation

1. Document your code in source files and the manual. (tm)

1. PHP is implemented in C99.  The optional fixed-width integers from
    stdint.h (int8_t, int16_t, int32_t, int64_t and their unsigned
    counterparts) must be available.

1. Functions that are given pointers to resources should not free them.

    For instance, `function int mail(char *to, char *from)` should NOT free `to`
    and/or `from`.

    Exceptions:

    * The function's designated behavior is freeing that resource. E.g.
      `efree()`

    * The function is given a boolean argument, that controls whether or not the
      function may free its arguments (if true, the function must free its
      arguments; if false, it must not)

    * Low-level parser routines, that are tightly integrated with the token
      cache and the bison code for minimum memory copying overhead.

1. Functions that are tightly integrated with other functions within the same
    module, and rely on each other's non-trivial behavior, should be documented as
    such and declared `static`. They should be avoided if possible.

1. Use definitions and macros whenever possible, so that constants have
    meaningful names and can be easily manipulated. Any use of a numeric
    constant to specify different behavior or actions should be done through
    a `#define`.

1. When writing functions that deal with strings, be sure to remember that PHP
    holds the length property of each string, and that it shouldn't be
    calculated with `strlen()`. Write your functions in such a way so that
    they'll take advantage of the length property, both for efficiency and in
    order for them to be binary-safe. Functions that change strings and obtain
    their new lengths while doing so, should return that new length, so it
    doesn't have to be recalculated with `strlen()` (e.g. `php_addslashes()`).

1. NEVER USE `strncat()`. If you're absolutely sure you know what you're doing,
    check its man page again, and only then, consider using it, and even then,
    try avoiding it.

1. Use `PHP_*` macros in the PHP source, and `ZEND_*` macros in the Zend part of
    the source. Although the `PHP_*` macros are mostly aliased to the `ZEND_*`
    macros it gives a better understanding on what kind of macro you're calling.

1. When commenting out code using a `#if` statement, do NOT use `0` only.
    Instead, use `"<git username here>_0"`. For example, `#if FOO_0`,
    where `FOO` is your git user `foo`. This allows easier tracking of why
    code was commented out, especially in bundled libraries.

1. Do not define functions that are not available. For instance, if a library is
    missing a function, do not define the PHP version of the function, and do
    not raise a run-time error about the function not existing. End users should
    use `function_exists()` to test for the existence of a function.

1. Prefer `emalloc()`, `efree()`, `estrdup()`, etc. to their standard C library
    counterparts. These functions implement an internal "safety-net" mechanism
    that ensures the deallocation of any unfreed memory at the end of a request.
    They also provide useful allocation and overflow information while running
    in debug mode.

    In almost all cases, memory returned to the engine must be allocated using
    `emalloc()`.

    The use of `malloc()` should be limited to cases where a third-party library
    may need to control or free the memory, or when the memory in question needs
    to survive between multiple requests.

1. The return type of "is" or "has" style functions should be `bool`,
    which return a "yes"/"no" answer.  `zend_result` is an appropriate
    return value for functions that perform some operation that may
    succeed or fail.

## User functions/methods naming conventions

1. Function names for user-level functions should be enclosed with in the
    `PHP_FUNCTION()` macro. They should be in lowercase, with words underscore
    delimited, with care taken to minimize the letter count. Abbreviations
    should not be used when they greatly decrease the readability of the
    function name itself:

    Good:

    ```php
    str_word_count
    array_key_exists
    ```

    Ok:

    ```php
    date_interval_create_from_date_string
    // Could be 'date_intvl_create_from_date_str'?
    get_html_translation_table()
    // Could be 'html_get_trans_table'?
    ```

    Bad:

    ```php
    hw_GetObjectByQueryCollObj
    pg_setclientencoding
    jf_n_s_i
    ```

1. If they are part of a "parent set" of functions, that parent should be
    included in the user function name, and should be clearly related to the
    parent program or function family. This should be in the form of `parent_*`:

    A family of `foo` functions, for example:

    Good:

    ```php
    foo_select_bar
    foo_insert_baz
    foo_delete_baz
    ```

    Bad:

    ```php
    fooselect_bar
    fooinsertbaz
    delete_foo_baz
    ```

1. Function names used by user functions should be prefixed with `_php_`, and
    followed by a word or an underscore-delimited list of words, in lowercase
    letters, that describes the function. If applicable, they should be declared
    `static`.

1. Variable names must be meaningful. One letter variable names must be avoided,
    except for places where the variable has no real meaning or a trivial
    meaning (e.g. `for (i=0; i<100; i++) ...`).

1. Variable names should be in lowercase. Use underscores to separate between
    words.

1. Method names follow the *studlyCaps* (also referred to as *bumpy case* or
    *camel caps*) naming convention, with care taken to minimize the letter
    count. The initial letter of the name is lowercase, and each letter that
    starts a new `word` is capitalized:

    Good:

    ```php
    connect()
    getData()
    buildSomeWidget()
    ```

    Bad:

    ```php
    get_Data()
    buildsomewidget()
    getI()
    ```

1. Class names should be descriptive nouns in *PascalCase* and as short as
    possible. Each word in the class name should start with a capital letter,
    without underscore delimiters. The class name should be prefixed with the
    name of the "parent set" (e.g. the name of the extension) if no namespaces
    are used. Abbreviations and acronyms as well as initialisms should be
    avoided wherever possible, unless they are much more widely used than the
    long form (e.g. HTTP or URL). Abbreviations start with a capital letter
    followed by lowercase letters, whereas acronyms and initialisms are written
    according to their standard notation. Usage of acronyms and initialisms is
    not allowed if they are not widely adopted and recognized as such.

    Good:

    ```php
    Curl
    CurlResponse
    HTTPStatusCode
    URL
    BTreeMap // B-tree Map
    Id // Identifier
    ID // Identity Document
    Char // Character
    Intl // Internationalization
    Radar // Radio Detecting and Ranging
    ```

    Bad:

    ```php
    curl
    curl_response
    HttpStatusCode
    Url
    BtreeMap
    ID // Identifier
    CHAR
    INTL
    RADAR // Radio Detecting and Ranging
    ```

## Internal function naming conventions

1. Functions that are part of the external API should be named
    `php_modulename_function()` to avoid symbol collision. They should be in
    lowercase, with words underscore delimited. Exposed API must be defined in
    `php_modulename.h`.

    ```c
    PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS);
    ```

    Unexposed module function should be static and should not be defined in
    `php_modulename.h`.

    ```c
    static int php_session_destroy()
    ```

1. Main module source file must be named `modulename.c`.

1. Header file that is used by other sources must be named `php_modulename.h`.

## Syntax and indentation

1. Use K&R-style. Of course, we can't and don't want to force anybody to use a
    style he or she is not used to, but, at the very least, when you write code
    that goes into the core of PHP or one of its standard modules, please
    maintain the K&R style. This applies to just about everything, starting with
    indentation and comment styles and up to function declaration syntax. Also
    see [Indentstyle](http://www.catb.org/~esr/jargon/html/I/indent-style.html).

1. Be generous with whitespace and braces. Keep one empty line between the
    variable declaration section and the statements in a block, as well as
    between logical statement groups in a block. Maintain at least one empty
    line between two functions, preferably two. Always prefer:

    ```c
    if (foo) {
        bar;
    }
    ```

    to:

    ```c
    if(foo)bar;
    ```

1. When indenting, use the tab character. A tab is expected to represent four
    spaces. It is important to maintain consistency in indentation so that
    definitions, comments, and control structures line up correctly.

1. Preprocessor statements (`#if` and such) MUST start at column one. To indent
    preprocessor directives you should put the `#` at the beginning of a line,
    followed by any number of spaces.

1. The length of constant string literals should be calculated via ``strlen()``
   instead of using ``sizeof()-1`` as it is clearer and any modern compiler
   will optimize it away. Legacy usages of the latter style exists within the
   codebase but should not be refactored, unless larger refactoring around that
   code is taking place.

## Testing

1. Extensions should be well tested using `*.phpt` tests. Read more at
    [qa.php.net](https://qa.php.net/write-test.php) documentation.

## New and experimental functions

To reduce the problems normally associated with the first public implementation
of a new set of functions, it has been suggested that the first implementation
include a file labeled `EXPERIMENTAL` in the function directory, and that the
functions follow the standard prefixing conventions during their initial
implementation.

The file labelled `EXPERIMENTAL` should include the following information:

* Any authoring information (known bugs, future directions of the module).
* Ongoing status notes which may not be appropriate for Git comments.

In general, new features should go to PECL or experimental branches until there
are specific reasons for directly adding them to the core distribution.

## Aliases & legacy documentation

You may also have some deprecated aliases with close to duplicate names, for
example, `somedb_select_result` and `somedb_selectresult`. For documentation
purposes, these will only be documented by the most current name, with the
aliases listed in the documentation for the parent function. For ease of
reference, user-functions with completely different names, that alias to the
same function (such as `highlight_file` and `show_source`), will be separately
documented.

Backwards compatible functions and names should be maintained as long as the
code can be reasonably be kept as part of the codebase. See the `README` in the
PHP documentation repository for more information on documentation.
