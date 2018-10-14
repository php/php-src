--TEST--
Test stripos() function : usage variations - unexpected inputs for 'needle' argument
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function with unexpected inputs for 'needle' and
 *  an expected type of input for 'haystack' argument
*/

echo "*** Testing stripos() function with unexpected values for needle ***\n";

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

  // null vlaues
  NULL,
  null,

  // resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop through each element of the 'needle' array to check the working of stripos()
$counter = 1;
for($index = 0; $index < count($needles); $index ++) {
  echo "\n-- Iteration $counter --\n";
  var_dump( stripos($haystack, $needles[$index]) );
  $counter ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing stripos() function with unexpected values for needle ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
bool(false)

-- Iteration 11 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
bool(false)

-- Iteration 12 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
bool(false)

-- Iteration 13 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
bool(false)

-- Iteration 14 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --
bool(false)

-- Iteration 19 --

Notice: Object of class sample could not be converted to int in %s on line %d
bool(false)

-- Iteration 20 --
bool(false)

-- Iteration 21 --
bool(false)

-- Iteration 22 --
bool(false)

-- Iteration 23 --
bool(false)

-- Iteration 24 --

Warning: stripos(): needle is not a string or an integer in %s on line %d
%s

-- Iteration 25 --
bool(false)

-- Iteration 26 --
bool(false)
*** Done ***
