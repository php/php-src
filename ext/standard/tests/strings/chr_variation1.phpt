--TEST--
Test chr() function : usage variations - test values for $ascii argument
--FILE--
<?php

echo "*** Testing chr() function: with unexpected inputs for 'ascii' argument ***\n";

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values for $input
$inputs =  array (

          // integer values
/*1*/	  0,
          1,
          255,
          256,

          // float values
/*5*/	  10.5,
          -20.5,
          1.1234e6,

          // boolean values
/*11*/	  true,
          false,
          TRUE,
          FALSE,
);

// loop through with each element of the $inputs array to test chr() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( bin2hex(chr($input)) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
--EXPECTF--
*** Testing chr() function: with unexpected inputs for 'ascii' argument ***
-- Iteration 1 --
string(2) "00"
-- Iteration 2 --
string(2) "01"
-- Iteration 3 --
string(2) "ff"
-- Iteration 4 --
string(2) "00"
-- Iteration 5 --

Deprecated: Implicit conversion from float 10.5 to int loses precision in %s on line %d
string(2) "0a"
-- Iteration 6 --

Deprecated: Implicit conversion from float -20.5 to int loses precision in %s on line %d
string(2) "ec"
-- Iteration 7 --
string(2) "48"
-- Iteration 8 --
string(2) "01"
-- Iteration 9 --
string(2) "00"
-- Iteration 10 --
string(2) "01"
-- Iteration 11 --
string(2) "00"
