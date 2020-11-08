--TEST--
Test strrchr() function : usage variations - unexpected inputs for needle
--FILE--
<?php
/* Test strrchr() function: with unexpected inputs for needle
 *  and expected type for haystack
*/

echo "*** Testing strrchr() function with unexpected inputs for needle ***\n";

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

$haystacks =  array (
  //integer numeric strings
  "0",
  "1",
  "2",
  "-2",

  //float numeric strings
  "10.5",
  "-10.5",
  "10.5e10",
  "10.6E-10",
  ".5",

  //regular strings
  "array",
  "a",
  "r",
  "y",
  "ay",
  "true",
  "false",
  "TRUE",
  "FALSE",
  "NULL",
  "null",
  "object",

  //empty string
  "",
  '',

  //resource variable in string form
  "\$file_handle",

  //undefined variable in string form
  @"$undefined_var",
  @"$unset_var"
);

// array with different values
$needles =  array (

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

  // null values
  NULL,
  null,

  // objects
  new sample(),

  // empty string
  "",
  '',

  // resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop through each element of the array and check the working of strrchr()
$count = 1;
for($index = 0; $index < count($haystacks); $index++) {
  echo "-- Iteration $count --\n";
  try {
    var_dump( strrchr($haystacks[$index], $needles[$index]) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function with unexpected inputs for needle ***
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
bool(false)
-- Iteration 4 --
string(2) "-2"
-- Iteration 5 --
string(4) "10.5"
-- Iteration 6 --
string(5) "-10.5"
-- Iteration 7 --
string(2) "10"
-- Iteration 8 --
string(2) "10"
-- Iteration 9 --
bool(false)
-- Iteration 10 --
strrchr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 11 --
strrchr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 12 --
strrchr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 13 --
strrchr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 14 --
strrchr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
string(6) "object"
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --
strrchr(): Argument #2 ($needle) must be of type string, resource given
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
*** Done ***
