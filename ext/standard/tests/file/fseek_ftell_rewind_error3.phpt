--TEST--
Test fseek(), ftell() & rewind() functions : error conditions - rewind()
--FILE--
<?php

/* Prototype: int fseek ( resource $handle, int $offset [, int $whence] );
   Description: Seeks on a file pointer

   Prototype: bool rewind ( resource $handle );
   Description: Rewind the position of a file pointer

   Prototype: int ftell ( resource $handle );
   Description: Tells file pointer read/write position
*/

echo "*** Testing rewind() : error conditions ***\n";
// zero argument
echo "-- Testing rewind() with zero argument --\n";
var_dump( rewind() );

// more than expected no. of args
echo "-- Testing rewind() with more than expected number of arguments --\n";
$fp = fopen(__FILE__, "r");
var_dump( rewind($fp, 10) );

// test invalid arguments : non-resources
echo "-- Testing rewind() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
);
/* loop to test rewind with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( rewind($invalid_args[$loop_counter - 1]) );
}

// rewind on a file handle which is already closed
echo "-- Testing rewind() with closed/unset file handle --";
fclose($fp);
var_dump(rewind($fp));

// rewind on a file handle which is unset
$file_handle = fopen(__FILE__, "r");
unset($file_handle); //unset file handle
var_dump( rewind(@$file_handle) );

echo "Done\n";
?>
--EXPECTF--
*** Testing rewind() : error conditions ***
-- Testing rewind() with zero argument --

Warning: Wrong parameter count for rewind() in %s on line %d
NULL
-- Testing rewind() with more than expected number of arguments --

Warning: Wrong parameter count for rewind() in %s on line %d
NULL
-- Testing rewind() with invalid arguments --
-- Iteration 1 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 2 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 3 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 4 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 5 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 6 --

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Testing rewind() with closed/unset file handle --
Warning: rewind(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: rewind(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
Done
