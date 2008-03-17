--TEST--
Test ctype_graph() function : error conditions - incorrect number of arguments
--FILE--
<?php
/* Prototype  : bool ctype_graph(mixed $c)
 * Description: Checks for any printable character(s) except space 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass an incorrect number of arguments to ctype_graph() to test behaviour
 */

echo "*** Testing ctype_graph() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ctype_graph() function with Zero arguments --\n";
var_dump( ctype_graph() );

//Test ctype_graph with one more than the expected number of arguments
echo "\n-- Testing ctype_graph() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_graph($c, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing ctype_graph() : error conditions ***

-- Testing ctype_graph() function with Zero arguments --

Warning: ctype_graph() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_graph() function with more than expected no. of arguments --

Warning: ctype_graph() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
