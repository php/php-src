--TEST--
Test array_fill_keys() function : variation of parameter 
--FILE--
<?php
/* Prototype  : proto array array_fill_keys(array keys, mixed val)
 * Description: Create an array using the elements of the first parameter as keys each initialized to val 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

/* Testing with unexpected argument types */

echo "*** Testing array_fill_keys() : parameter variations ***\n";

$simpleStr = "simple";
$fp = fopen(__FILE__, "r");
$bool = false;
$float = 2.4;
$array = array("one", "two");
$nullVal = null;

echo "\n-- Testing array_fill_keys() function with both wrong arguments --\n";
var_dump( array_fill_keys($bool, $float) );

echo "\n-- Testing array_fill_keys() function with unusual second arguments --\n";
var_dump( array_fill_keys($array, $fp) );

echo "\n-- Testing array_fill_keys() function with mixed array --\n";
var_dump( array_fill_keys($nullVal, $simpleStr) );

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with both wrong arguments --

Warning: array_fill_keys() expects parameter 1 to be array, boolean given in %sarray_fill_keys_variation3.php on line %d
NULL

-- Testing array_fill_keys() function with unusual second arguments --
array(2) {
  ["one"]=>
  resource(%d) of type (stream)
  ["two"]=>
  resource(%d) of type (stream)
}

-- Testing array_fill_keys() function with mixed array --

Warning: array_fill_keys() expects parameter 1 to be array, null given in %sarray_fill_keys_variation3.php on line %d
NULL
Done