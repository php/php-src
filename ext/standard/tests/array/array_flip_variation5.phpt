--TEST--
Test array_flip() function : usage variations - 'input' argument with repeatitive keys and values
--FILE--
<?php
/* Prototype  : array array_flip(array $input)
 * Description: Return array with key <-> value flipped 
 * Source code: ext/standard/array.c
*/

/*
* Using different types of repeatitive keys as well as values for 'input' array
*/

echo "*** Testing array_flip() : 'input' array with repeatitive keys/values ***\n";

// array with numeric key repeatition
$input = array(1 => 'value', 2 => 'VALUE', 1 => "VaLuE", 3.4 => 4, 3.4 => 5);
var_dump( array_flip($input) );

// array with string key repeatition
$input = array("key" => 1, "two" => 'TWO', 'three' => 3, 'key' => "FOUR");
var_dump( array_flip($input) );

// array with bool key repeatition
$input = array(true => 1, false => 0, TRUE => -1);
var_dump( array_flip($input) );

// array with null key repeatition
$input = array(null => "Hello", NULL => 0);
var_dump( array_flip($input) );

// array with numeric value repeatition
$input = array('one' => 1, 'two' => 2, 3 => 1, "index" => 1);
var_dump( array_flip($input) );

//array with string value repeatition
$input = array('key1' => "value1", "key2" => '2', 'key3' => 'value1');
var_dump( array_flip($input) );

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : 'input' array with repeatitive keys/values ***
array(3) {
  ["VaLuE"]=>
  int(1)
  ["VALUE"]=>
  int(2)
  [5]=>
  int(3)
}
array(3) {
  ["FOUR"]=>
  string(3) "key"
  ["TWO"]=>
  string(3) "two"
  [3]=>
  string(5) "three"
}
array(2) {
  [-1]=>
  int(1)
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  string(0) ""
}
array(2) {
  [1]=>
  string(5) "index"
  [2]=>
  string(3) "two"
}
array(2) {
  ["value1"]=>
  string(4) "key3"
  [2]=>
  string(4) "key2"
}
Done
