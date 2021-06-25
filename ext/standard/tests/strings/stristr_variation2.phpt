--TEST--
Test stristr() function : usage variations - test values for $needle argument
--FILE--
<?php

echo "*** Testing stristr() function: with unexpected inputs for 'needle' argument ***\n";

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
          -2,
          -PHP_INT_MAX,

          // float values
/*5*/	  10.5,
          -20.5,
          10.1234567e10,

          // array values
/*8*/	  array(),
          array(0),
          array(1, 2),

          // boolean values
/*11*/	  true,
          false,
          TRUE,
          FALSE,

          // objects
/*17*/	  new sample(),

          // resource
/*18*/	  $file_handle,
);

//defining '$pad_length' argument
$pad_length = "20";

// loop through with each element of the $inputs array to test stristr() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  try {
    var_dump( stristr("Hello World", $input) );
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
--EXPECTF--
*** Testing stristr() function: with unexpected inputs for 'needle' argument ***
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
stristr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 9 --
stristr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 10 --
stristr(): Argument #2 ($needle) must be of type string, array given
-- Iteration 11 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
-- Iteration 12 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(11) "Hello World"
-- Iteration 13 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
-- Iteration 14 --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(11) "Hello World"
-- Iteration 15 --
bool(false)
-- Iteration 16 --
stristr(): Argument #2 ($needle) must be of type string, resource given
