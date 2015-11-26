--TEST--
Test is_resource() function : error conditions 
--FILE--
<?php
/* Prototype  : bool is_resource  ( mixed $var  )
 * Description:  Finds whether a variable is a resource
 * Source code: ext/standard/type.c
 */

echo "*** Testing is_resource() : error conditions ***\n";

echo "\n-- Testing is_resource() function with Zero arguments --\n";
var_dump( is_resource() );

echo "\n-- Testing is_resource() function with more than expected no. of arguments --\n";
$res = fopen(__FILE__, "r");
$extra_arg = 10;
var_dump( is_resource($res, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing is_resource() : error conditions ***

-- Testing is_resource() function with Zero arguments --

Warning: is_resource() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing is_resource() function with more than expected no. of arguments --

Warning: is_resource() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===