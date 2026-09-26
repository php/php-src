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

          // boolean values
/*8*/	  true,
          false,

          // objects
/*10*/	  new sample(),
);

//defining '$pad_length' argument
$pad_length = "20";

// loop through with each element of the $inputs array to test stristr() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( stristr("Hello World", $input) );
  $count ++;
}

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
bool(false)
-- Iteration 9 --
string(11) "Hello World"
-- Iteration 10 --
bool(false)
