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
string(1) "0"
string(1) "0"

-- Iteration 2 --
string(1) "1"
string(1) "1"

-- Iteration 3 --
string(5) "12345"
string(5) "12345"

-- Iteration 4 --
string(5) "-2345"
string(5) "-2345"

-- Iteration 5 --
string(4) "10.5"
string(4) "10.5"

-- Iteration 6 --
string(5) "-10.5"
string(5) "-10.5"

-- Iteration 7 --
string(12) "101234567000"
string(12) "101234567000"

-- Iteration 8 --
string(13) "1.07654321E-9"
string(13) "1.07654321E-9"

-- Iteration 9 --
string(3) "0.5"
string(3) "0.5"

-- Iteration 10 --
string(5) "Array"
string(5) "Array"

-- Iteration 11 --
string(5) "Array"
string(5) "Array"

-- Iteration 12 --
string(5) "Array"
string(5) "Array"

-- Iteration 13 --
string(5) "Array"
string(5) "Array"

-- Iteration 14 --
string(5) "Array"
string(5) "Array"

-- Iteration 15 --
string(0) ""
string(0) ""

-- Iteration 16 --
string(0) ""
string(0) ""

-- Iteration 17 --
string(1) "1"
string(1) "1"

-- Iteration 18 --
string(0) ""
string(0) ""

-- Iteration 19 --
string(1) "1"
string(1) "1"

-- Iteration 20 --
string(0) ""
string(0) ""

-- Iteration 21 --
string(0) ""
string(0) ""

-- Iteration 22 --
string(0) ""
string(0) ""

-- Iteration 23 --
string(6) "string"
string(6) "string"

-- Iteration 24 --
string(6) "string"
string(6) "string"

-- Iteration 25 --
string(6) "Object"
string(6) "Object"

-- Iteration 26 --
string(0) ""
string(0) ""

-- Iteration 27 --
string(0) ""
string(0) ""

-- Iteration 28 --
string(%d) "Resource id #%d"
string(%d) "Resource id #%d"
Done