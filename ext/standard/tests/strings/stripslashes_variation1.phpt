--TEST--
Test stripslashes() function : usage variations  - non-string type argument 
--FILE--
<?php
/* Prototype  : string stripslashes ( string $str )
 * Description: Returns an un-quoted string
 * Source code: ext/standard/string.c
*/

/*
 * Test stripslashes() with non-string type argument such as int, float, etc 
*/

echo "*** Testing stripslashes() : with non-string type argument ***\n";
// initialize all required variables

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
class sample  {
  public function __toString() {
  return "obj\'ct";
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

  // NULL values 
  NULL,
  null
);


// loop through each element of the array and check the working of stripslashes()
// when $str arugment is supplied with different values
echo "\n--- Testing stripslashes() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( stripslashes($str) );

  $counter ++;
}

// closing the file
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing stripslashes() : with non-string type argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing stripslashes() by supplying different values for 'str' argument ---
-- Iteration 1 --
unicode(1) "0"
-- Iteration 2 --
unicode(1) "1"
-- Iteration 3 --
unicode(5) "12345"
-- Iteration 4 --
unicode(5) "-2345"
-- Iteration 5 --
unicode(4) "10.5"
-- Iteration 6 --
unicode(5) "-10.5"
-- Iteration 7 --
unicode(12) "105000000000"
-- Iteration 8 --
unicode(7) "1.06E-9"
-- Iteration 9 --
unicode(3) "0.5"
-- Iteration 10 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(1) "1"
-- Iteration 16 --
unicode(0) ""
-- Iteration 17 --
unicode(1) "1"
-- Iteration 18 --
unicode(0) ""
-- Iteration 19 --
unicode(0) ""
-- Iteration 20 --
unicode(0) ""
-- Iteration 21 --
unicode(0) ""
-- Iteration 22 --
unicode(0) ""
-- Iteration 23 --
unicode(6) "obj'ct"
-- Iteration 24 --

Warning: stripslashes() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
-- Iteration 25 --
unicode(0) ""
-- Iteration 26 --
unicode(0) ""
Done
