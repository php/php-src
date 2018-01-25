--TEST--
Test restore_error_handler() function : bug #46196
--CREDITS--
Olivier Doucet
--FILE--
<?php
/* Prototype  : void restore_error_handler(void)
 * Description: Restores the previously defined error handler function
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing restore_error_handler() : error bug #46196 ***\n";

var_dump( set_error_handler( 'myErrorHandler' ) );
var_dump( restore_error_handler() );
var_dump( set_error_handler( 'myErrorHandler' ) );

function myErrorHandler($errno, $errstr, $errfile, $errline)
{
    return true;
}

?>
===DONE===
--EXPECTF--
*** Testing restore_error_handler() : error bug #46196 ***
NULL
bool(true)
NULL
===DONE===
