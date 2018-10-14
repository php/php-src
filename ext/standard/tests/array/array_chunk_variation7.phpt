--TEST--
Test array_chunk() function : usage variations - references
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
 *            : Chunks an array into size large chunks
 * Source code: ext/standard/array.c
*/

/*
 * Testing array_chunk() function with following conditions
 *   1. input array containing references
*/

echo "*** Testing array_chunk() : usage variations ***\n";

$size = 2;

echo "\n-- Testing array_chunk(), input array containing references \n";

$numbers=array(1, 2, 3, 4);
// reference array
$input_array = array (
  "one" => &$numbers[0],
  "two" => &$numbers[1],
  "three" => &$numbers[2],
  "four" => &$numbers[3]
);

var_dump( array_chunk($input_array, $size) );
var_dump( array_chunk($input_array, $size, true) );
var_dump( array_chunk($input_array, $size, false) );

echo "Done";
?>
--EXPECTF--
*** Testing array_chunk() : usage variations ***

-- Testing array_chunk(), input array containing references 
array(2) {
  [0]=>
  array(2) {
    [0]=>
    &int(1)
    [1]=>
    &int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    &int(3)
    [1]=>
    &int(4)
  }
}
array(2) {
  [0]=>
  array(2) {
    ["one"]=>
    &int(1)
    ["two"]=>
    &int(2)
  }
  [1]=>
  array(2) {
    ["three"]=>
    &int(3)
    ["four"]=>
    &int(4)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    &int(1)
    [1]=>
    &int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    &int(3)
    [1]=>
    &int(4)
  }
}
Done
