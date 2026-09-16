--TEST--
Test chr() function : usage variations - test values for $ascii argument
--FILE--
<?php

echo "*** Testing chr() function: with unexpected inputs for 'ascii' argument ***\n";

// array with different values for $input
$inputs = [
	0,
	1,
	255,
	// float values
	10.5,
	// bool values
	true,
	false,
];

// loop through with each element of the $inputs array to test chr() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( bin2hex(chr($input)) );
  $count ++;
}

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

Deprecated: Implicit conversion from float 10.5 to int loses precision in %s on line %d
string(2) "0a"
-- Iteration 5 --
string(2) "01"
-- Iteration 6 --
string(2) "00"
