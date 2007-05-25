--TEST--
Test fgetc() function : error conditions
--FILE--
<?php
/*
 Prototype: string fgetc ( resource $handle );
 Description: Gets character from file pointer
*/

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fgetc() with zero argument --\n";
var_dump( fgetc() );

// more than expected no. of args
echo "-- Testing fgetc() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
var_dump( fgetc($fp, $fp) );
fclose($fp);

// test invalid arguments : non-resources
echo "-- Testing fgetc() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fgetc() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fgetc($invalid_args[$loop_counter - 1]) );
}

echo "Done\n";
--EXPECTF--
*** Testing error conditions ***
-- Testing fgetc() with zero argument --

Warning: Wrong parameter count for fgetc() in %s on line %d
NULL
-- Testing fgetc() with more than expected number of arguments --

Warning: Wrong parameter count for fgetc() in %s on line %d
NULL
-- Testing fgetc() with invalid arguments --
-- Iteration 1 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %s
bool(false)
-- Iteration 6 --

Warning: fgetc(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
Done
