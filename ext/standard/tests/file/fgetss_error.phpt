--TEST--
Test fgetss() function : error conditions
--FILE--
<?php
/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fgetss() with zero argument --\n";
var_dump( fgetss() );

// more than expected no. of args
echo "-- Testing fgetss() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
var_dump( fgetss($fp, 100, '<p><a>', $fp) );

// invalid length argument 
echo "-- Testing fgetss() with invalid length arguments --\n";
$len = 0; 
$allowable_tags = '<p><a>';
var_dump( fgetss($fp, $len, $allowable_tags) );
$len = -10;
var_dump( fgetss($fp, $len, $allowable_tags) );
$len = 1; 
var_dump( fgetss($fp, $len, $allowable_tags) ); // return length - 1 always, expect false

// test invalid arguments : non-resources
echo "-- Testing fgetss() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fgetss() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fgetss($invalid_args[$loop_counter - 1], 10, $allowable_tags) );
}
// fgetss() on a file handle which is already closed
echo "-- Testing fgetss() with closed/unset file handle --";
fclose($fp);
var_dump(fgetss($fp,10,$allowable_tags));

// fgetss() on a file handle which is unset
$file_handle = fopen(__FILE__, "r");
unset($file_handle); //unset file handle
var_dump( fgetss(@$file_handle,10));

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions ***
-- Testing fgetss() with zero argument --

Warning: fgetss() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
-- Testing fgetss() with more than expected number of arguments --

Warning: fgetss() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
-- Testing fgetss() with invalid length arguments --

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fgetss(): Length parameter must be greater than 0 in %s on line %d
bool(false)
bool(false)
-- Testing fgetss() with invalid arguments --
-- Iteration 1 --

Warning: fgetss() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fgetss() expects parameter 1 to be resource, integer given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fgetss() expects parameter 1 to be resource, double given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fgetss() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fgetss() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fgetss() expects parameter 1 to be resource, object given in %s on line %d
bool(false)
-- Testing fgetss() with closed/unset file handle --
Warning: fgetss(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: fgetss() expects parameter 1 to be resource, null given in %s on line %d
bool(false)
Done
