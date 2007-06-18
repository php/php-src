--TEST--
Test fread() function : error conditions
--FILE--
<?php
/*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle. 
   Reading stops when up to length bytes have been read, EOF (end of file) is 
   reached, (for network streams) when a packet becomes available, or (after 
   opening userspace stream) when 8192 bytes have been read whichever comes first.
*/

echo "*** Testing error conditions ***\n";
$filename = __FILE__; 
$file_handle = fopen($filename, "r");

// zero argument
echo "-- Testing fread() with zero argument --\n";
var_dump( fread() );

// more than expected no. of args
echo "-- Testing fread() with more than expected number of arguments --\n";
var_dump( fread($file_handle, 10, $file_handle) );

// invalid length argument 
echo "-- Testing fread() with invalid length arguments --\n";
$len = 0; 
var_dump( fread($file_handle, $len) );
$len = -10;
var_dump( fread($file_handle, $len) );

// test invalid arguments : non-resources
echo "-- Testing fread() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test fread() with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fread($invalid_args[$loop_counter - 1], 10) );
}

// fwrite() on a file handle which is already closed
echo "-- Testing fwrite() with closed/unset file handle --\n";
fclose($file_handle);
var_dump( fread($file_handle,$file_content_type) );

// fwrite on a file handle which is unset
$fp = fopen($filename, "r");
unset($fp); //unset file handle
var_dump( fread(@$fp,10) );
var_dump( fclose(@$fp) );

echo "Done\n";
--EXPECTF--
*** Testing error conditions ***
-- Testing fread() with zero argument --

Warning: Wrong parameter count for fread() in %s on line %d
NULL
-- Testing fread() with more than expected number of arguments --

Warning: Wrong parameter count for fread() in %s on line %d
NULL
-- Testing fread() with invalid length arguments --

Warning: fread(): Length parameter must be greater than 0 in %s on line %d
bool(false)

Warning: fread(): Length parameter must be greater than 0 in %s on line %d
bool(false)
-- Testing fread() with invalid arguments --
-- Iteration 1 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Testing fwrite() with closed/unset file handle --

Notice: Undefined variable: file_content_type in %s on line %d

Warning: fread(): 5 is not a valid stream resource in %s on line %d
bool(false)

Warning: fread(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
Done
