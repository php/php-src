--TEST--
Test stripos() function : usage variations - unexpected inputs for 'haystack' and 'needle' arguments
--FILE--
<?php
/* Test stripos() function with unexpected inputs for 'haystack' and 'needle' arguments */

echo "*** Testing stripos() function with unexpected values for haystack and needle ***\n";

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// defining a class
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
  10.5e10,
  10.6E-10,
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


// loop through each element of the array and check the working of stripos()
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $haystack = $values[$index];
  try {
    var_dump( stripos($values[$index], $values[$index]) );
  } catch (Error $e) {
    echo get_class($e) . ": " . $e->getMessage(), "\n";
  }
  try {
    var_dump( stripos($values[$index], $values[$index], 1) );
  } catch (Error $e) {
    echo get_class($e) . ": " . $e->getMessage(), "\n";
  }
  $counter ++;
}

echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function with unexpected values for haystack and needle ***
-- Iteration 1 --
int(0)
bool(false)
-- Iteration 2 --
int(0)
bool(false)
-- Iteration 3 --
int(0)
bool(false)
-- Iteration 4 --
int(0)
bool(false)
-- Iteration 5 --
int(0)
bool(false)
-- Iteration 6 --
int(0)
bool(false)
-- Iteration 7 --
int(0)
bool(false)
-- Iteration 8 --
int(0)
bool(false)
-- Iteration 9 --
int(0)
bool(false)
-- Iteration 10 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 11 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 12 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 13 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 14 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, array given
-- Iteration 15 --
int(0)
bool(false)
-- Iteration 16 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 17 --
int(0)
bool(false)
-- Iteration 18 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 19 --
int(0)
bool(false)
-- Iteration 20 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 21 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 22 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 23 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 24 --
TypeError: stripos(): Argument #1 ($haystack) must be of type string, resource given
TypeError: stripos(): Argument #1 ($haystack) must be of type string, resource given
-- Iteration 25 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
-- Iteration 26 --
int(0)
ValueError: stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
*** Done ***
