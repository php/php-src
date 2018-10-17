--TEST--
Test ctype_cntrl() function : error conditions - Incorrect number of args
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_cntrl(mixed $c)
 * Description: Checks for control character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass an incorrect number of arguments to ctype_cntrl() to test behaviour
 */

echo "*** Testing ctype_cntrl() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ctype_cntrl() function with Zero arguments --\n";
var_dump( ctype_cntrl() );

//Test ctype_cntrl with one more than the expected number of arguments
echo "\n-- Testing ctype_cntrl() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_cntrl($c, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing ctype_cntrl() : error conditions ***

-- Testing ctype_cntrl() function with Zero arguments --

Warning: ctype_cntrl() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_cntrl() function with more than expected no. of arguments --

Warning: ctype_cntrl() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
