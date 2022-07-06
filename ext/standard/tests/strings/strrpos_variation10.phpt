--TEST--
Test strrpos() function : usage variations - unexpected inputs for 'needle' argument
--FILE--
<?php
/* Test strrpos() function with unexpected inputs for 'needle' and
 *  an expected type of input for 'haystack' argument
*/

echo "*** Testing strrpos() function with unexpected values for needle ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

$haystack = "string 0 1 2 -2 10.5 -10.5 10.5e10 10.6E-10 .5 array true false object \"\" null Resource";

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

// loop through each element of the 'needle' array to check the working of strrpos()
$counter = 1;
for($index = 0; $index < count($needles); $index ++) {
  echo "-- Iteration $counter --\n";
  try {
    var_dump( strrpos($haystack, $needles[$index]) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $counter ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function with unexpected values for needle ***
-- Iteration 1 --
int(42)
-- Iteration 2 --
int(41)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
int(27)
-- Iteration 6 --
int(21)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
int(28)
-- Iteration 10 --
strrpos(): Argument #2 ($needle) must be of type string, array given
-- Iteration 11 --
strrpos(): Argument #2 ($needle) must be of type string, array given
-- Iteration 12 --
strrpos(): Argument #2 ($needle) must be of type string, array given
-- Iteration 13 --
strrpos(): Argument #2 ($needle) must be of type string, array given
-- Iteration 14 --
strrpos(): Argument #2 ($needle) must be of type string, array given
-- Iteration 15 --
int(41)
-- Iteration 16 --
int(87)
-- Iteration 17 --
int(41)
-- Iteration 18 --
int(87)
-- Iteration 19 --
int(64)
-- Iteration 20 --
int(87)
-- Iteration 21 --
int(87)
-- Iteration 22 --
int(87)
-- Iteration 23 --
int(87)
-- Iteration 24 --
strrpos(): Argument #2 ($needle) must be of type string, resource given
-- Iteration 25 --
int(87)
-- Iteration 26 --
int(87)
*** Done ***
