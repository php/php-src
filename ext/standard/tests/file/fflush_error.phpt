--TEST--
Test fflush() function: error conditions
--FILE--
<?php
/*
 Prototype: bool fflush ( resource $handle );
 Description: Flushes the output to a file
*/

echo "*** Testing error conditions ***\n";
$file_path = dirname(__FILE__);

// zero argument
echo "-- Testing fflush(): with zero argument --\n";
var_dump( fflush() );

// more than expected no. of args
echo "-- Testing fflush(): with more than expected number of arguments --\n";

$filename = "$file_path/fflush_error.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");
   
var_dump( fflush($file_handle, $file_handle) );
fclose($file_handle);

// test invalid arguments : non-resources
echo "-- Testing fflush(): with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass
);

/* loop to test fflush() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fflush($invalid_args[$loop_counter - 1]) );
}
echo "\n*** Done ***";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/fflush_error.tmp");
?>

--EXPECTF--
*** Testing error conditions ***
-- Testing fflush(): with zero argument --

Warning: fflush() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
-- Testing fflush(): with more than expected number of arguments --

Warning: fflush() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
-- Testing fflush(): with invalid arguments --
-- Iteration 1 --

Warning: fflush() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fflush() expects parameter 1 to be resource, integer given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fflush() expects parameter 1 to be resource, float given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fflush() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fflush() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fflush() expects parameter 1 to be resource, object given in %s on line %d
bool(false)

*** Done ***

