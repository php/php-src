--TEST--
Test array_product() function : variation - using a keyed array
--FILE--
<?php
/* Prototype  : mixed array_product(array input)
 * Description: Returns the product of the array entries 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_product() : variations ***\n";

echo "\n-- Testing array_product() function with a keyed array --\n";
var_dump( array_product(array("bob" => 2, "janet" => 5)) );
?>
===DONE===
--EXPECTF--
*** Testing array_product() : variations ***

-- Testing array_product() function with a keyed array --
int(10)
===DONE===
