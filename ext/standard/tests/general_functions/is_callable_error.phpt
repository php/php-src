--TEST--
Test is_callable() function
--INI--
precision=14
error_reporting = E_ALL & ~E_NOTICE | E_STRICT
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

Warning: Wrong parameter count for is_callable() in %s on line %d
NULL

-- Testing is_callable() function with more than expected no. of arguments --

Warning: Wrong parameter count for is_callable() in %s on line %d
NULL
===DONE===