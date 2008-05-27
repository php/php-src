--TEST--
Test vsprintf() function : usage variations - unexpected values for the format argument
--FILE--
<?php
/* Prototype  : string vsprintf(string $format, array $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vsprintf() when different unexpected format strings are passed to
 * the '$format' argument of the function
*/

echo "*** Testing vsprintf() : with unexpected values for format argument ***\n";

// initialising the required variables
$args = array(1, 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring a class
class sample
{
  public function __toString() {
  return "object";
  }
}

// Defining resource
$file_handle = fopen(__FILE__, 'r');


//array of values to iterate over
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array data
  array(),
  array(0),
  array(1),
  array(1,2),
  array('color' => 'red', 'item' => 'pen'),

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // object data
  new sample(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,
 
  // resource data
  $file_handle
);

// loop through each element of the array for format

$counter = 1;
foreach($values as $value) {
  echo "\n -- Iteration $counter --\n";
  var_dump( vsprintf($value,$args) );
  $counter++;
    
};

// closing the resource
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : with unexpected values for format argument ***

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
unicode(12) "101234567000"

 -- Iteration 8 --
unicode(13) "1.07654321E-9"

 -- Iteration 9 --
unicode(3) "0.5"

 -- Iteration 10 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

 -- Iteration 11 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

 -- Iteration 12 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

 -- Iteration 13 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

 -- Iteration 14 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Array"

 -- Iteration 15 --
unicode(0) ""

 -- Iteration 16 --
unicode(0) ""

 -- Iteration 17 --
unicode(1) "1"

 -- Iteration 18 --
unicode(0) ""

 -- Iteration 19 --
unicode(1) "1"

 -- Iteration 20 --
unicode(0) ""

 -- Iteration 21 --
unicode(0) ""

 -- Iteration 22 --
unicode(0) ""

 -- Iteration 23 --
unicode(6) "object"

 -- Iteration 24 --
unicode(0) ""

 -- Iteration 25 --
unicode(0) ""

 -- Iteration 26 --
unicode(%d) "Resource id #%d"
Done
