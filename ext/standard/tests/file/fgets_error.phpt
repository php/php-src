--TEST--
Test fgets() function : error conditions
--FILE--
<?php
/*
 Prototype: string fgets ( resource $handle [, int $length] );
 Description: Gets line from file pointer
*/

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fgets() with zero argument --\n";
var_dump( fgets() );

// more than expected no. of args
echo "-- Testing fgets() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
var_dump( fgets($fp, 10, $fp) );

// invalid length argument
echo "-- Testing fgets() with invalid length arguments --\n";
$len = 0;
var_dump( fgets($fp, $len) );
$len = -10;
var_dump( fgets($fp, $len) );
$len = 1;
var_dump( fgets($fp, $len) ); // return length - 1 always, expect false


// test invalid arguments : non-resources
echo "-- Testing fgets() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fgets() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fgets($invalid_args[$loop_counter - 1], 10) );
}

// fgets() on a file handle which is already closed
echo "-- Testing fgets() with closed/unset file handle --";
fclose($fp);
var_dump(fgets($fp,10));

// fgets() on a file handle which is unset
$file_handle = fopen(__FILE__, "r");
unset($file_handle); //unset file handle
var_dump( fgets(@$file_handle,10));

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
-- Testing fgets() with zero argument --

Warning: fgets() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
-- Testing fgets() with more than expected number of arguments --

Warning: fgets() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
-- Testing fgets() with invalid length arguments --

Warning: fgets(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fgets(): Length parameter must be greater than 0 in %s on line %d
bool(false)
bool(false)
-- Testing fgets() with invalid arguments --
-- Iteration 1 --

Warning: fgets() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fgets() expects parameter 1 to be resource, integer given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fgets() expects parameter 1 to be resource, float given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fgets() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fgets() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fgets() expects parameter 1 to be resource, object given in %s on line %d
bool(false)
-- Testing fgets() with closed/unset file handle --
Warning: fgets(): supplied resource is not a valid stream resource in %s on line %d
bool(false)

Warning: fgets() expects parameter 1 to be resource, null given in %s on line %d
bool(false)
Done
