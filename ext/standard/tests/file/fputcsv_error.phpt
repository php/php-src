--TEST--
Test fputcsv() function : error conditions
--FILE--
<?php
/*
 Prototype: int fputcsv ( resource $handle [, array $fields [, string $delimiter [, string $enclosure]]] );
 Description:fputcsv() formats a line (passed as a fields array) as CSV and write it to the specified file
   handle. Returns the length of the written string, or FALSE on failure.
*/

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fputcsv() with zero argument --\n";
var_dump( fputcsv() );

// more than expected no. of args
echo "-- Testing fputcsv() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
$fields = array("fld1", "fld2");
$delim = ";";
$enclosure ="\"";
var_dump( fputcsv($fp, $fields, $delim, $enclosure, $fp) );
fclose($fp);

// test invalid arguments : non-resources
echo "-- Testing fputcsv() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fputcsv() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fputcsv($invalid_args[$loop_counter - 1]) ); // with default args
  var_dump( fputcsv($invalid_args[$loop_counter - 1], $fields, $delim, $enclosure) ); // all args specified
}

echo "Done\n";
--EXPECTF--
*** Testing error conditions ***
-- Testing fputcsv() with zero argument --

Warning: fputcsv() expects at least 2 parameters, 0 given in %s on line %d
NULL
-- Testing fputcsv() with more than expected number of arguments --

Warning: fputcsv() expects parameter 5 to be string, resource given in %s on line %d
NULL
-- Testing fputcsv() with invalid arguments --
-- Iteration 1 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, string given in %s on line %d
NULL
-- Iteration 2 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, int given in %s on line %d
NULL
-- Iteration 3 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, float given in %s on line %d
NULL
-- Iteration 4 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, bool given in %s on line %d
NULL
-- Iteration 5 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, array given in %s on line %d
NULL
-- Iteration 6 --

Warning: fputcsv() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fputcsv() expects parameter 1 to be resource, object given in %s on line %d
NULL
Done
