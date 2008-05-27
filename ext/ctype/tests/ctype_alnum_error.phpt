--TEST--
Test ctype_alnum() function : error conditions - Incorrect number of args
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_alnum(mixed $c)
 * Description: Checks for alphanumeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass incorrect number of arguments to ctype_alnum() to test behaviour
 */

echo "*** Testing ctype_alnum() : error conditions ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

// Zero arguments
echo "\n-- Testing ctype_alnum() function with Zero arguments --\n";
var_dump( ctype_alnum() );

//Test ctype_alnum with one more than the expected number of arguments
echo "\n-- Testing ctype_alnum() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_alnum($c, $extra_arg) );

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alnum() : error conditions ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_error.php on line %d

-- Testing ctype_alnum() function with Zero arguments --

Warning: ctype_alnum() expects exactly 1 parameter, 0 given in %sctype_alnum_error.php on line 17
NULL

-- Testing ctype_alnum() function with more than expected no. of arguments --

Warning: ctype_alnum() expects exactly 1 parameter, 2 given in %sctype_alnum_error.php on line 23
NULL

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_error.php on line %d
===DONE===
