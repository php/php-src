--TEST--
Test ctype_space() function : error conditions - Incorrect number of args
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_space(mixed $c)
 * Description: Checks for whitespace character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass an incorrect number of arguments to ctype_space() to test behaviour
 */

echo "*** Testing ctype_space() : error conditions ***\n";

$orig = setlocale(LC_CTYPE, "C");

// Zero arguments
echo "\n-- Testing ctype_space() function with Zero arguments --\n";
var_dump( ctype_space() );

//Test ctype_space with one more than the expected number of arguments
echo "\n-- Testing ctype_space() function with more than expected no. of arguments --\n";
$c = " ";
$extra_arg = 10;
var_dump( ctype_space($c, $extra_arg) );

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_space() : error conditions ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_space_error.php on line 13

-- Testing ctype_space() function with Zero arguments --

Warning: ctype_space() expects exactly 1 parameter, 0 given in %sctype_space_error.php on line 17
NULL

-- Testing ctype_space() function with more than expected no. of arguments --

Warning: ctype_space() expects exactly 1 parameter, 2 given in %sctype_space_error.php on line 23
NULL

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_space_error.php on line 25
===DONE===
