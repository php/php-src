--TEST--
Test ctype_punct() function : error conditions - incorrect number of args
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_punct(mixed $c)
 * Description: Checks for any printable character which is not whitespace
 * or an alphanumeric character
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass incorrect number of arguments to ctype_punct() to test behaviour
 */

echo "*** Testing ctype_punct() : error conditions ***\n";

$orig = setlocale(LC_CTYPE, "C");

// Zero arguments
echo "\n-- Testing ctype_punct() function with Zero arguments --\n";
var_dump( ctype_punct() );

//Test ctype_punct with one more than the expected number of arguments
echo "\n-- Testing ctype_punct() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_punct($c, $extra_arg) );

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_punct() : error conditions ***

-- Testing ctype_punct() function with Zero arguments --

Warning: ctype_punct() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_punct() function with more than expected no. of arguments --

Warning: ctype_punct() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
