--TEST--
Test ob_get_contents() function : error cases
--CREDITS--
Iain Lewis <ilewis@php.net>
--FILE--
<?php
/* Prototype  : proto string ob_get_contents(void)
 * Description: Return the contents of the output buffer
 * Source code: main/output.c
 * Alias to functions:
 */


echo "*** Testing ob_get_contents() : error cases ***\n";

var_dump(ob_get_contents("bob"));

ob_start();

var_dump(ob_get_contents("bob2",345));

echo "Done\n";
?>
--EXPECTF--
*** Testing ob_get_contents() : error cases ***

Warning: ob_get_contents() expects exactly 0 parameters, 1 given in %s on line 11
NULL

Warning: ob_get_contents() expects exactly 0 parameters, 2 given in %s on line 15
NULL
Done
