--TEST--
Test array_combine() function : basic functionality
--FILE--
<?php
echo "*** Testing array_combine() : basic functionality ***\n";

/* Different arrays for $keys and $values arguments */

// array with default keys for $keys and $values arguments
$keys_array = array(1, 2);
$values_array = array(3,4);
var_dump( array_combine($keys_array, $values_array) );

// associative arrays for $keys and $values arguments
$keys_array = array(1 => "a", 2 => 'b');
$values_array = array(3 => 'c', 4 => "d");
var_dump( array_combine($keys_array, $values_array) );

// mixed array for $keys and $values arguments
$keys_array = array(1, 2 => "b");
$values_array = array(3 => 'c', 4);
var_dump( array_combine($keys_array, $values_array) );

echo "Done";
?>
--EXPECT--
*** Testing array_combine() : basic functionality ***
array(2) {
  [1]=>
  int(3)
  [2]=>
  int(4)
}
array(2) {
  ["a"]=>
  string(1) "c"
  ["b"]=>
  string(1) "d"
}
array(2) {
  [1]=>
  string(1) "c"
  ["b"]=>
  int(4)
}
Done
