--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - ftell()
--FILE--
<?php

/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

echo "*** Testing ftell() : error conditions ***\n";
// zero argument
echo "-- Testing ftell() with zero argument --\n";
var_dump( ftell() );

// more than expected no. of args
echo "-- Testing ftell() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
var_dump( ftell($fp, 10) );

// test invalid arguments : non-resources
echo "-- Testing ftell() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test ftell with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( ftell($invalid_args[$loop_counter - 1]) );
}

// ftell on a file handle which is already closed
echo "-- Testing ftell with closed/unset file handle --";
fclose($fp);
var_dump(ftell($fp));

// ftell on a file handle which is unset
$file_handle = fopen(__FILE__, "r");
unset($file_handle); //unset file handle
var_dump( ftell(@$file_handle) );

echo "Done\n";
?>
--EXPECTF--
*** Testing ftell() : error conditions ***
-- Testing ftell() with zero argument --

Warning: ftell() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
-- Testing ftell() with more than expected number of arguments --

Warning: ftell() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
-- Testing ftell() with invalid arguments --
-- Iteration 1 --

Warning: ftell() expects parameter 1 to be resource, string given in %s on line %d
bool(false)
-- Iteration 2 --

Warning: ftell() expects parameter 1 to be resource, integer given in %s on line %d
bool(false)
-- Iteration 3 --

Warning: ftell() expects parameter 1 to be resource, float given in %s on line %d
bool(false)
-- Iteration 4 --

Warning: ftell() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
-- Iteration 5 --

Warning: ftell() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
-- Iteration 6 --

Warning: ftell() expects parameter 1 to be resource, object given in %s on line %d
bool(false)
-- Testing ftell with closed/unset file handle --
Warning: ftell(): supplied resource is not a valid stream resource in %s on line %d
bool(false)

Warning: ftell() expects parameter 1 to be resource, null given in %s on line %d
bool(false)
Done
