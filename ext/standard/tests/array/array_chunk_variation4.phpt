--TEST--
Test array_chunk() function : usage variations - array with diff. sub arrays
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
 *            : Chunks an array into size large chunks
 * Source code: ext/standard/array.c
*/

/*
* Testing array_chunk() function - input array containing different sub arrays
*/

echo "*** Testing array_chunk() : usage variations ***\n";

$size = 2;

// input array
$input_array = array (
  "array1" => array(),
  "array2" => array(1, 2, 3),
  "array3" =>  array(1)
);

echo "\n-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' as defualt --\n";
var_dump( array_chunk($input_array, $size) );

echo "\n-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' = true --\n";
var_dump( array_chunk($input_array, $size, true) );

echo "\n-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' = false --\n";
var_dump( array_chunk($input_array, $size, false) );

echo "Done";
?>
--EXPECTF--
*** Testing array_chunk() : usage variations ***

-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' as defualt --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}

-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' = true --
array(2) {
  [0]=>
  array(2) {
    ["array1"]=>
    array(0) {
    }
    ["array2"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
  }
  [1]=>
  array(1) {
    ["array3"]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}

-- Testing array_chunk() by supplying an array containing different sub arrays & 'preserve_key' = false --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
  }
  [1]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}
Done
