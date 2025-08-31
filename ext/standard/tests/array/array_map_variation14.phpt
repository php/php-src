--TEST--
Test array_map() function : usage variations - null value for 'callback' argument
--FILE--
<?php
/*
 * Test array_map() by passing null values for $callback argument and testing whether shortest
 * array will be extended with empty elements
 */

echo "*** Testing array_map() : null value for 'callback' argument ***\n";

// arrays to be passed as arguments
$arr1 = array(1, 2);
$arr2 = array("one", "two");
$arr3 = array(1.1, 2.2);

// get an unset variable
$unset_var = 10;
unset ($unset_var);

/* calling array_map() with null callback */

echo "-- with null --\n";
var_dump( array_map(null, $arr1, $arr2, $arr3) );
var_dump( array_map(NULL, $arr1, $arr2, $arr3) );

echo "-- with unset variable --\n";
var_dump( array_map(@$unset_var, $arr1, $arr2, $arr3) );

echo "-- with undefined variable --\n";
var_dump( array_map(@$undefined_var, $arr1) );

echo "-- with empty string --\n";
try {
    var_dump( array_map("", $arr1, $arr2) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- with empty array --\n";
try {
    var_dump( array_map(array(), $arr1, $arr2) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_map() : null value for 'callback' argument ***
-- with null --
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    string(3) "one"
    [2]=>
    float(1.1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(2)
    [1]=>
    string(3) "two"
    [2]=>
    float(2.2)
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    string(3) "one"
    [2]=>
    float(1.1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(2)
    [1]=>
    string(3) "two"
    [2]=>
    float(2.2)
  }
}
-- with unset variable --
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    string(3) "one"
    [2]=>
    float(1.1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(2)
    [1]=>
    string(3) "two"
    [2]=>
    float(2.2)
  }
}
-- with undefined variable --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- with empty string --
array_map(): Argument #1 ($callback) must be a valid callback or null, function "" not found or invalid function name
-- with empty array --
array_map(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
Done
