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

echo "\n-- Testing property_exists() function with incorrect arguments --\n";
$property_name = 'string_val';
var_dump( property_exists(10, $property_name) );

?>
--EXPECTF--
*** Testing property_exists() : error conditions ***

-- Testing property_exists() function with incorrect arguments --

Warning: First parameter must either be an object or the name of an existing class in %sproperty_exists_error.php on line %d
NULL
