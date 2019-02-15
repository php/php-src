--TEST--
Test natcasesort() function : usage variations - referenced variables
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array of referenced varaibles to test how natcasesort() re-orders it
 */

echo "*** Testing natcasesort() : usage variation ***\n";

$value1 = 100;
$value2 = 33;
$value3 = 555;

echo "\n-- Initial test --\n";
$array =  array( &$value1 , &$value2, &$value3);
var_dump( natcasesort($array) );
var_dump($array);

echo "\n-- Change \$value1 --\n";
$value1 = -29;
var_dump( natcasesort($array) );
var_dump($array);

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variation ***

-- Initial test --
bool(true)
array(3) {
  [1]=>
  &int(33)
  [0]=>
  &int(100)
  [2]=>
  &int(555)
}

-- Change $value1 --
bool(true)
array(3) {
  [0]=>
  &int(-29)
  [1]=>
  &int(33)
  [2]=>
  &int(555)
}
Done
