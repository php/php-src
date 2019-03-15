--TEST--
Test fwrite() function : error conditions
--FILE--
<?php
/*
 Prototype: int fwrite ( resource $handle,string string, [, int $length] );
 Description: fwrite() writes the contents of string to the file stream pointed to by handle.
              If the length arquement is given,writing will stop after length bytes have been
              written or the end of string reached, whichever comes first.
              fwrite() returns the number of bytes written or FALSE on error
*/

// include the file.inc for Function: function delete_file($filename)
include ("file.inc");

echo "*** Testing fwrite() : error conditions ***\n";

$filename = __DIR__."/fwrite_error.tmp";

echo "-- Testing fwrite() with less than expected number of arguments --\n";
// zero argument
var_dump( fwrite() );
// less than expected, 1 arg
$file_handle  = fopen ( $filename, "w");
var_dump( fwrite($file_handle) );

// more than expected no. of args
echo "-- Testing fwrite() with more than expected number of arguments --\n";
$data = "data";
var_dump( fwrite($file_handle, $data, strlen($data), 10) );

// invalid length argument
echo "-- Testing fwrite() with invalid length arguments --\n";
$len = 0;
var_dump( fwrite($file_handle, $data, $len) );
$len = -10;
var_dump( fwrite($file_handle, $data, $len) );

// test invalid arguments : non-resources
echo "-- Testing fwrite() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fwrite() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fwrite($invalid_args[$loop_counter - 1], 10) );
}

// fwrite() on a file handle which is already closed
echo "-- Testing fwrite() with closed/unset file handle --\n";
fclose($file_handle);
var_dump(fwrite($file_handle,"data"));

// fwrite on a file handle which is unset
$fp = fopen($filename, "w");
unset($fp); //unset file handle
var_dump( fwrite(@$fp,"data"));

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/fwrite_error.tmp";
unlink( $filename );
?>
--EXPECTF--
*** Testing fwrite() : error conditions ***
-- Testing fwrite() with less than expected number of arguments --

Warning: fwrite() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

Warning: fwrite() expects at least 2 parameters, 1 given in %s on line %d
bool(false)
-- Testing fwrite() with more than expected number of arguments --

Warning: fwrite() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
-- Testing fwrite() with invalid length arguments --
int(0)
int(0)
-- Testing fwrite() with invalid arguments --
-- Iteration 1 --

Warning: fwrite() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fwrite() expects parameter 1 to be resource, int given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fwrite() expects parameter 1 to be resource, float given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fwrite() expects parameter 1 to be resource, bool given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fwrite() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fwrite() expects parameter 1 to be resource, object given in %s on line %d
bool(false)
-- Testing fwrite() with closed/unset file handle --

Warning: fwrite(): supplied resource is not a valid stream resource in %s on line %d
bool(false)

Warning: fwrite() expects parameter 1 to be resource, null given in %s on line %d
bool(false)
Done
