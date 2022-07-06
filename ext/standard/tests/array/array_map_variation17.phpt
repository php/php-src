--TEST--
Test array_map() function : usage variations - unexpected values for 'callback' argument
--FILE--
<?php
/*
 * Test array_map() by passing different scalar/nonscalar values in place of $callback
 */

echo "*** Testing array_map() : unexpected values for 'callback' argument ***\n";

$arr1 = array(1, 2, 3);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $input argument
$unexpected_callbacks = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // boolean data
/*10*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*14*/ "",
       '',

       // array data
/*16*/ array(),
       array(1, 2),
       array(1, array(2)),

       // object data
/*19*/ new classA(),

       // resource variable
/*20*/ $fp
);

// loop through each element of $inputs to check the behavior of array_map
for($count = 0; $count < count($unexpected_callbacks); $count++) {
    echo "\n-- Iteration ".($count + 1)." --\n";
    try {
        var_dump( array_map($unexpected_callbacks[$count], $arr1));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

fclose($fp);
echo "Done";
?>
--EXPECT--
*** Testing array_map() : unexpected values for 'callback' argument ***

-- Iteration 1 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 2 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 3 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 4 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 5 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 6 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 7 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 8 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 9 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 10 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 11 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 12 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 13 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 14 --
array_map(): Argument #1 ($callback) must be a valid callback, function "" not found or invalid function name

-- Iteration 15 --
array_map(): Argument #1 ($callback) must be a valid callback, function "" not found or invalid function name

-- Iteration 16 --
array_map(): Argument #1 ($callback) must be a valid callback, array must have exactly two members

-- Iteration 17 --
array_map(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object

-- Iteration 18 --
array_map(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object

-- Iteration 19 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given

-- Iteration 20 --
array_map(): Argument #1 ($callback) must be a valid callback, no array or string given
Done
