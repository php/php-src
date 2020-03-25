--TEST--
Test vsprintf() function : usage variations - unexpected values for args argument
--FILE--
<?php
/* Prototype  : string vsprintf(string format, array args)
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vsprintf() when different unexpected values are passed to
 * the '$args' arguments of the function
*/

echo "*** Testing vsprintf() : with unexpected values for args argument ***\n";

// initialising the required variables
$format = '%s';

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

// loop through each element of the array for args
$counter = 1;
foreach($values as $value) {
  echo "\n-- Iteration $counter --\n";
  try {
    var_dump( vsprintf($format,$value) );
  } catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
  }
  $counter++;
};

// closing the resource
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : with unexpected values for args argument ***

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
string(12) "101234567000"

-- Iteration 8 --
string(13) "1.07654321E-9"

-- Iteration 9 --
string(3) "0.5"

-- Iteration 10 --
The arguments array must contain 1 items, 0 given

-- Iteration 11 --
The arguments array must contain 1 items, 0 given

-- Iteration 12 --
string(1) "1"

-- Iteration 13 --
string(0) ""

-- Iteration 14 --
string(1) "1"

-- Iteration 15 --
string(0) ""

-- Iteration 16 --
string(0) ""

-- Iteration 17 --
string(0) ""

-- Iteration 18 --
string(6) "string"

-- Iteration 19 --
string(6) "string"

-- Iteration 20 --
The arguments array must contain 1 items, 0 given

-- Iteration 21 --
The arguments array must contain 1 items, 0 given

-- Iteration 22 --
The arguments array must contain 1 items, 0 given

-- Iteration 23 --
string(%d) "Resource id #%d"
Done
