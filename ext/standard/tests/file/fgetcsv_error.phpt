--TEST--
Test fgetcsv() function : error conditions
--FILE--
<?php
/*
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fgetcsv() with zero argument --\n";
var_dump( fgetcsv() );

// more than expected no. of args
echo "-- Testing fgetcsv() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
$len = 1024;
$delim = ";";
$enclosure ="\"";
var_dump( fgetcsv($fp, $len, $delim, $enclosure, $fp) );
fclose($fp);

// test invalid arguments : non-resources
echo "-- Testing fgetcsv() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fgetcsv() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fgetcsv($invalid_args[$loop_counter - 1]) ); // with default args
  var_dump( fgetcsv($invalid_args[$loop_counter - 1], $len, $delim, $enclosure) ); // all args specified
}

echo "Done\n";
--EXPECTF--
*** Testing error conditions ***
-- Testing fgetcsv() with zero argument --

Warning: fgetcsv() expects at least 1 parameter, 0 given in %s on line %d
NULL
-- Testing fgetcsv() with more than expected number of arguments --

Warning: fgetcsv() expects at most 4 parameters, 5 given in %s on line %d
NULL
-- Testing fgetcsv() with invalid arguments --
-- Iteration 1 --

Warning: fgetcsv() expects parameter 1 to be resource, string given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, string given in %s on line %d
NULL
-- Iteration 2 --

Warning: fgetcsv() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, integer given in %s on line %d
NULL
-- Iteration 3 --

Warning: fgetcsv() expects parameter 1 to be resource, double given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, double given in %s on line %d
NULL
-- Iteration 4 --

Warning: fgetcsv() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, boolean given in %s on line %d
NULL
-- Iteration 5 --

Warning: fgetcsv() expects parameter 1 to be resource, array given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, array given in %s on line %d
NULL
-- Iteration 6 --

Warning: fgetcsv() expects parameter 1 to be resource, object given in %s on line %d
NULL

Warning: fgetcsv() expects parameter 1 to be resource, object given in %s on line %d
NULL
Done
