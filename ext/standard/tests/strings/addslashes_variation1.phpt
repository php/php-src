--TEST--
Test addslashes() function : usage variations  - non-string type argument 
--INI--
--FILE--
<?php
/* Prototype  : string addslashes ( string $str )
 * Description: Returns a string with backslashes before characters that need to be quoted in database queries etc.
 * Source code: ext/standard/string.c
*/

/*
 * Test addslashes() with non-string type argument such as int, float, etc 
*/

echo "*** Testing addslashes() : with non-string type argument ***\n";
// initialize all required variables

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
class sample  {
  public function __toString() {
  return "obj'ct";
  } 
}

// Defining resource
$file_handle = fopen(__FILE__, 'r');

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

  // empty string
  "",
  '',

  // undefined variable
  $undefined_var,

  // unset variable
  $unset_var,
  
  // objects
  new sample(),

  // resource
  $file_handle,
 
  NULL,
  null
);


// loop through each element of the array and check the working of addslashes()
// when $str arugment is supplied with different values
echo "\n--- Testing addslashes() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( addslashes($str) );

  $counter ++;
}

// closing the file
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing addslashes() : with non-string type argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing addslashes() by supplying different values for 'str' argument ---
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
string(12) "105000000000"
-- Iteration 8 --
string(7) "1.06E-9"
-- Iteration 9 --
string(3) "0.5"
-- Iteration 10 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 11 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 12 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 13 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 14 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 15 --
string(1) "1"
-- Iteration 16 --
string(0) ""
-- Iteration 17 --
string(1) "1"
-- Iteration 18 --
string(0) ""
-- Iteration 19 --
string(0) ""
-- Iteration 20 --
string(0) ""
-- Iteration 21 --
string(0) ""
-- Iteration 22 --
string(0) ""
-- Iteration 23 --
string(7) "obj\'ct"
-- Iteration 24 --
string(%d) "Resource id #%d"
-- Iteration 25 --
string(0) ""
-- Iteration 26 --
string(0) ""
Done
