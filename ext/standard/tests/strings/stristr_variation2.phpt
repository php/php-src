--TEST--
Test stristr() function : usage variations - test values for $needle argument
--FILE--
<?php

echo "*** Testing stristr() function: with unexpected inputs for 'needle' argument ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

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

          // null values
/*15*/	  NULL,
          null,

          // objects
/*17*/	  new sample(),

          // resource
/*18*/	  $file_handle,

          // undefined variable
/*19*/	  @$undefined_var,

          // unset variable
/*20*/	  @$unset_var
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
--EXPECT--
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
bool(false)
-- Iteration 12 --
string(11) "Hello World"
-- Iteration 13 --
bool(false)
-- Iteration 14 --
string(11) "Hello World"
-- Iteration 15 --
string(11) "Hello World"
-- Iteration 16 --
string(11) "Hello World"
-- Iteration 17 --
bool(false)
-- Iteration 18 --
stristr(): Argument #2 ($needle) must be of type string, resource given
-- Iteration 19 --
string(11) "Hello World"
-- Iteration 20 --
string(11) "Hello World"
