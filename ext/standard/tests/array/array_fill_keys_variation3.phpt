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

$fp = fopen(__FILE__, "r");
$array = array("one", "two");

echo "\n-- Testing array_fill_keys() function with unusual second arguments --\n";
var_dump( array_fill_keys($array, $fp) );

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with unusual second arguments --
array(2) {
  ["one"]=>
  resource(%d) of type (stream)
  ["two"]=>
  resource(%d) of type (stream)
}
Done
