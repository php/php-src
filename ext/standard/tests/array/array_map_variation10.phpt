--TEST--
Test array_map() function : usage variations - anonymous callback function
--FILE--
<?php
/*
 * Test array_map() by passing anonymous callback function with following variations
 */

echo "*** Testing array_map() : anonymous callback function ***\n";

$array1 = array(1, 2, 3);
$array2 = array(3, 4, 5);

echo "-- anonymous function with all parameters and body --\n";
var_dump( array_map( function($a, $b) { return array($a, $b); }, $array1, $array2));

echo "-- anonymous function with two parameters and passing one array --\n";
try {
    var_dump( array_map( function($a, $b) { return array($a, $b); }, $array1));
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "-- anonymous function with NULL parameter --\n";
var_dump( array_map( function() { return NULL; }, $array1));

echo "-- anonymous function with NULL body --\n";
var_dump( array_map( function($a) { }, $array1));

echo "-- passing NULL as 'array1' --\n";
try {
    var_dump( array_map( function($a) { return array($a); }, NULL));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : anonymous callback function ***
-- anonymous function with all parameters and body --
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(3)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(4)
  }
  [2]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(5)
  }
}
-- anonymous function with two parameters and passing one array --
Exception: Too few arguments to function {closure:%s:%d}(), 1 passed and exactly 2 expected
-- anonymous function with NULL parameter --
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
-- anonymous function with NULL body --
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
-- passing NULL as 'array1' --
array_map(): Argument #2 ($array) must be of type array, null given
Done
