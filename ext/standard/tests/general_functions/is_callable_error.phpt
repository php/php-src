--TEST--
Test is_callable() function
--FILE--
<?php
/* Prototype: bool is_callable ( mixed $var [, bool $syntax_only [, string &$callable_name]] );
   Description: Verify that the contents of a variable can be called as a function
                In case of objects, $var = array($SomeObject, 'MethodName')
*/

echo "\n*** Testing error conditions ***\n";

echo "\n-- Testing is_callable() function with less than expected no. of arguments --\n";
var_dump( is_callable() );

echo "\n-- Testing is_callable() function with more than expected no. of arguments --\n";
var_dump( is_callable("string", TRUE, $callable_name, "EXTRA") );

?>
===DONE===
--EXPECTF--
*** Testing error conditions ***

-- Testing is_callable() function with less than expected no. of arguments --

Warning: is_callable() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing is_callable() function with more than expected no. of arguments --

Warning: is_callable() expects at most 3 parameters, 4 given in %s on line %d
NULL
===DONE===
