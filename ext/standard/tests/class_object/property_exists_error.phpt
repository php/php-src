--TEST--
Test property_exists() function : error conditions
--FILE--
<?php
/* Prototype  : bool property_exists(mixed object_or_class, string property_name)
 * Description: Checks if the object or class has a property
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing property_exists() : error conditions ***\n";

$object_or_class = "obj";
$property_name = 'string_val';
$extra_arg = 10;


echo "\n-- Testing property_exists() function with more than expected no. of arguments --\n";
var_dump( property_exists($object_or_class, $property_name, $extra_arg) );


echo "\n-- Testing property_exists() function with less than expected no. of arguments --\n";
var_dump( property_exists($object_or_class) );

echo "\n-- Testing property_exists() function with incorrect arguments --\n";
var_dump( property_exists(10, $property_name) );

?>
===DONE===
--EXPECTF--
*** Testing property_exists() : error conditions ***

-- Testing property_exists() function with more than expected no. of arguments --

Warning: property_exists() expects exactly 2 parameters, 3 given in %sproperty_exists_error.php on line %d
NULL

-- Testing property_exists() function with less than expected no. of arguments --

Warning: property_exists() expects exactly 2 parameters, 1 given in %sproperty_exists_error.php on line %d
NULL

-- Testing property_exists() function with incorrect arguments --

Warning: First parameter must either be an object or the name of an existing class in %sproperty_exists_error.php on line %d
NULL
===DONE===
