--TEST--
Test sprintf() function : usage variations - with all types of values for arg1 argument
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

error_reporting(E_ALL & ~E_NOTICE);

echo "*** Testing sprintf() : with different types of values passed for arg1 argument ***\n";

// initialing required variables
$format = '%s';
$arg2 = 'third argument';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring class
class sample
{
  public function __toString() {
    return "Object";
  } 
}

// creating a file resource
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
      array(1, 2),
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

      // string data
      "string",
      'string',

      // object data
      new sample(),

      // undefined data
      @$undefined_var,

      // unset data
      @$unset_var,

      // resource data
      $file_handle
);

// loop through each element of the array for arg1

$count = 1;
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";
  
  // with two arguments
  var_dump( sprintf($format, $value) );

  // with three arguments
  var_dump( sprintf($format, $value, $arg2) );
  
  $count++;   
};

// closing the resource
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : with different types of values passed for arg1 argument ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "0"

-- Iteration 2 --
unicode(1) "1"
unicode(1) "1"

-- Iteration 3 --
unicode(5) "12345"
unicode(5) "12345"

-- Iteration 4 --
unicode(5) "-2345"
unicode(5) "-2345"

-- Iteration 5 --
unicode(4) "10.5"
unicode(4) "10.5"

-- Iteration 6 --
unicode(5) "-10.5"
unicode(5) "-10.5"

-- Iteration 7 --
unicode(12) "101234567000"
unicode(12) "101234567000"

-- Iteration 8 --
unicode(13) "1.07654321E-9"
unicode(13) "1.07654321E-9"

-- Iteration 9 --
unicode(3) "0.5"
unicode(3) "0.5"

-- Iteration 10 --
unicode(5) "Array"
unicode(5) "Array"

-- Iteration 11 --
unicode(5) "Array"
unicode(5) "Array"

-- Iteration 12 --
unicode(5) "Array"
unicode(5) "Array"

-- Iteration 13 --
unicode(5) "Array"
unicode(5) "Array"

-- Iteration 14 --
unicode(5) "Array"
unicode(5) "Array"

-- Iteration 15 --
unicode(0) ""
unicode(0) ""

-- Iteration 16 --
unicode(0) ""
unicode(0) ""

-- Iteration 17 --
unicode(1) "1"
unicode(1) "1"

-- Iteration 18 --
unicode(0) ""
unicode(0) ""

-- Iteration 19 --
unicode(1) "1"
unicode(1) "1"

-- Iteration 20 --
unicode(0) ""
unicode(0) ""

-- Iteration 21 --
unicode(0) ""
unicode(0) ""

-- Iteration 22 --
unicode(0) ""
unicode(0) ""

-- Iteration 23 --
unicode(6) "string"
unicode(6) "string"

-- Iteration 24 --
unicode(6) "string"
unicode(6) "string"

-- Iteration 25 --
unicode(6) "Object"
unicode(6) "Object"

-- Iteration 26 --
unicode(0) ""
unicode(0) ""

-- Iteration 27 --
unicode(0) ""
unicode(0) ""

-- Iteration 28 --
unicode(%d) "Resource id #%d"
unicode(%d) "Resource id #%d"
Done
