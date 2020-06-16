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

try {
    var_dump( property_exists(10, $property_name) );
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing property_exists() : error conditions ***

-- Testing property_exists() function with incorrect arguments --
property_exists(): Argument #1 ($object_or_class) must be of type object|string, int given
