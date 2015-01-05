--TEST--
Test preg_grep() function : error conditions -  wrong numbers of parameters
--FILE--
<?php

/* Prototype  : int preg_last_error  ( void  )
 * Description:  Returns the error code of the last PCRE regex execution
 * Source code: ext/pcre/php_pcre.c
 */

/*
 * Pass an incorrect number of arguments to preg_last_error() to test behaviour
 */

echo "*** Testing preg_last_error() : error conditions ***\n";

// Test preg_last_error with one more than the expected number of arguments
echo "\n-- Testing preg_last_error() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( preg_last_error($extra_arg) );
?>
===Done===
--EXPECTF--
*** Testing preg_last_error() : error conditions ***

-- Testing preg_last_error() function with more than expected no. of arguments --

Warning: preg_last_error() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===Done===