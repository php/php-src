# Input filter support in PHP

XSS (Cross Site Scripting) hacks are becoming more and more prevalent, and can
be quite difficult to prevent. Whenever you accept user data and somehow display
this data back to users, you are likely vulnerable to XSS hacks.

The Input Filter support in PHP is aimed at providing the framework through
which a company-wide or site-wide security policy can be enforced. It is
implemented as a SAPI hook and is called from the `treat_data` and post handler
functions. To implement your own security policy you will need to write a
standard PHP extension. There is also a powerful standard implementation in
`ext/filter` that should suit most peoples' needs. However, if you want to
implement your own security policy, read on.

A simple implementation might look like the following. This stores the original
raw user data and adds a `my_get_raw()` function while the normal `$_POST`,
`$_GET` and `$_COOKIE` arrays are only populated with stripped data. In this
simple example all I am doing is calling `strip_tags()` on the data.

```c
ZEND_BEGIN_MODULE_GLOBALS(my_input_filter)
        zval *post_array;
        zval *get_array;
        zval *cookie_array;
ZEND_END_MODULE_GLOBALS(my_input_filter)

#ifdef ZTS
#define IF_G(v) TSRMG(my_input_filter_globals_id, zend_my_input_filter_globals *, v)
#else
#define IF_G(v) (my_input_filter_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(my_input_filter)

zend_function_entry my_input_filter_functions[] = {
    PHP_FE(my_get_raw,   NULL)
    {NULL, NULL, NULL}
};

zend_module_entry my_input_filter_module_entry = {
    STANDARD_MODULE_HEADER,
    "my_input_filter",
    my_input_filter_functions,
    PHP_MINIT(my_input_filter),
    PHP_MSHUTDOWN(my_input_filter),
    NULL,
    PHP_RSHUTDOWN(my_input_filter),
    PHP_MINFO(my_input_filter),
    "0.1",
    STANDARD_MODULE_PROPERTIES
};

PHP_MINIT_FUNCTION(my_input_filter)
{
    ZEND_INIT_MODULE_GLOBALS(my_input_filter, php_my_input_filter_init_globals, NULL);

    REGISTER_LONG_CONSTANT("POST", PARSE_POST, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("GET", PARSE_GET, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("COOKIE", PARSE_COOKIE, CONST_CS | CONST_PERSISTENT);

    sapi_register_input_filter(my_sapi_input_filter);
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(my_input_filter)
{
    if(IF_G(get_array)) {
        zval_ptr_dtor(&IF_G(get_array));
        IF_G(get_array) = NULL;
    }
    if(IF_G(post_array)) {
        zval_ptr_dtor(&IF_G(post_array));
        IF_G(post_array) = NULL;
    }
    if(IF_G(cookie_array)) {
        zval_ptr_dtor(&IF_G(cookie_array));
        IF_G(cookie_array) = NULL;
    }
    return SUCCESS;
}

PHP_MINFO_FUNCTION(my_input_filter)
{
    php_info_print_table_start();
    php_info_print_table_row( 2, "My Input Filter Support", "enabled" );
    php_info_print_table_end();
}

/* The filter handler. If you return 1 from it, then PHP also registers the
 * (modified) variable. Returning 0 prevents PHP from registering the variable;
 * you can use this if your filter already registers the variable under a
 * different name, or if you just don't want the variable registered at all. */
SAPI_INPUT_FILTER_FUNC(my_sapi_input_filter)
{
    zval new_var;
    zval *array_ptr = NULL;
    char *raw_var;
    int var_len;

    assert(*val != NULL);

    switch(arg) {
        case PARSE_GET:
            if(!IF_G(get_array)) {
                ALLOC_ZVAL(array_ptr);
                array_init(array_ptr);
                INIT_PZVAL(array_ptr);
            }
            IF_G(get_array) = array_ptr;
            break;
        case PARSE_POST:
            if(!IF_G(post_array)) {
                ALLOC_ZVAL(array_ptr);
                array_init(array_ptr);
                INIT_PZVAL(array_ptr);
            }
            IF_G(post_array) = array_ptr;
            break;
        case PARSE_COOKIE:
            if(!IF_G(cookie_array)) {
                ALLOC_ZVAL(array_ptr);
                array_init(array_ptr);
                INIT_PZVAL(array_ptr);
            }
            IF_G(cookie_array) = array_ptr;
            break;
    }
    Z_STRLEN(new_var) = val_len;
    Z_STRVAL(new_var) = estrndup(*val, val_len);
    Z_TYPE(new_var) = IS_STRING;

    var_len = strlen(var);
    raw_var = emalloc(var_len+5);  /* RAW_ and a \0 */
    strcpy(raw_var, "RAW_");
    strlcat(raw_var,var,var_len+5);

    php_register_variable_ex(raw_var, &new_var, array_ptr);

    php_strip_tags(*val, val_len, NULL, NULL, 0);

    *new_val_len = strlen(*val);
    return 1;
}

PHP_FUNCTION(my_get_raw)
{
    long arg;
    char *var;
    int var_len;
    zval **tmp;
    zval *array_ptr = NULL;

    if(zend_parse_parameters(2, "ls", &arg, &var, &var_len) == FAILURE) {
        return;
    }

    switch(arg) {
        case PARSE_GET:
            array_ptr = IF_G(get_array);
            break;
        case PARSE_POST:
            array_ptr = IF_G(post_array);
            break;
        case PARSE_COOKIE:
            array_ptr = IF_G(post_array);
            break;
    }

    if(!array_ptr) {
        RETURN_FALSE;
    }

    if(zend_hash_find(HASH_OF(array_ptr), var, var_len+5, (void **)&tmp) == SUCCESS) {
        *return_value = **tmp;
        zval_copy_ctor(return_value);
    } else {
        RETVAL_FALSE;
    }
}
```
