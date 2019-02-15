--TEST--
Test array_product() function : variation
--FILE--
<?php
/* Prototype  : mixed array_product(array input)
 * Description: Returns the product of the array entries
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_product() : variations ***\n";

echo "\n-- Testing array_product() function with a very large array --\n";

$array = array();

for ($i = 0; $i < 999; $i++) {
  $array[] = 999;
}

var_dump( array_product($array) );
?>
===DONE===
--EXPECT--
*** Testing array_product() : variations ***

-- Testing array_product() function with a very large array --
float(INF)
===DONE===
