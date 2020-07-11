--TEST--
Test strrchr() function : usage variations - unexpected inputs for haystack and needle
--FILE--
<?php
/* Test strrchr() function with unexpected inputs for haystack and needle */

echo "*** Testing strrchr() function: with unexpected inputs for haystack and needle ***\n";

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
class sample  {
  public function __toString() {
    return "object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values
$values =  array (

  // integer values
  0,
  1,
  12345,
  -2345,

  // float values
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // objects
  new sample(),

  // empty string
  "",
  '',

  // null values
  NULL,
  null,

  // resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of strrchr()
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  try {
    var_dump( strrchr($values[$index], $values[$index]) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $counter ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with unexpected inputs for haystack and needle ***
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
string(5) "12345"
-- Iteration 4 --
string(5) "-2345"
-- Iteration 5 --
string(4) "10.5"
-- Iteration 6 --
string(5) "-10.5"
-- Iteration 7 --
string(10) "1234567000"
-- Iteration 8 --
string(4) "1E-9"
-- Iteration 9 --
string(3) "0.5"
-- Iteration 10 --
strrchr(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 11 --
strrchr(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 12 --
strrchr(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 13 --
strrchr(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 14 --
strrchr(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 15 --
string(1) "1"
-- Iteration 16 --
bool(false)
-- Iteration 17 --
string(1) "1"
-- Iteration 18 --
bool(false)
-- Iteration 19 --
string(6) "object"
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --
strrchr(): Argument #1 ($haystack) must be of type string, resource given
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
*** Done ***
