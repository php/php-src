--TEST--
Test array_map() function : usage variations - arrays of different size
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing array having different size
 *   1) first array as empty array
 *   2) second array as empty array
 *   3) second array shorter than first array
 *   4) first array shorter than second array
 *   5) one more array than callback function arguments
 */

echo "*** Testing array_map() : arrays with diff. size ***\n";

function callback($a, $b)
{
  return array($a => $b);
}

// calling array_map with different arrays
var_dump( array_map('callback', array(1, 2, 3), array()) );
var_dump( array_map('callback', array(), array('a', 'b', 'c')) );
var_dump( array_map('callback', array(1, 2, 3), array('a', 'b')) );
var_dump( array_map('callback', array(012, 0x2F, 0X1A), array(2.3, 12.4e2)) );
var_dump( array_map('callback', array(), array(1, 2, 3), array('a', 'b')) );  // passing more no. of arrays than callback function argument

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : arrays with diff. size ***
array(3) {
  [0]=>
  array(1) {
    [1]=>
    NULL
  }
  [1]=>
  array(1) {
    [2]=>
    NULL
  }
  [2]=>
  array(1) {
    [3]=>
    NULL
  }
}
array(3) {
  [0]=>
  array(1) {
    [""]=>
    string(1) "a"
  }
  [1]=>
  array(1) {
    [""]=>
    string(1) "b"
  }
  [2]=>
  array(1) {
    [""]=>
    string(1) "c"
  }
}
array(3) {
  [0]=>
  array(1) {
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(1) {
    [2]=>
    string(1) "b"
  }
  [2]=>
  array(1) {
    [3]=>
    NULL
  }
}
array(3) {
  [0]=>
  array(1) {
    [10]=>
    float(2.3)
  }
  [1]=>
  array(1) {
    [47]=>
    float(1240)
  }
  [2]=>
  array(1) {
    [26]=>
    NULL
  }
}
array(3) {
  [0]=>
  array(1) {
    [""]=>
    int(1)
  }
  [1]=>
  array(1) {
    [""]=>
    int(2)
  }
  [2]=>
  array(1) {
    [""]=>
    int(3)
  }
}
Done
