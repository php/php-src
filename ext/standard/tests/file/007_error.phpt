--TEST--
Test fopen, fclose() & feof() functions: error conditions
--FILE--
<?php
/*
 Prototype: resource fopen(string $filename, string $mode
                            [, bool $use_include_path [, resource $context]] );
 Description: Opens file or URL.

 Prototype: bool fclose ( resource $handle );
 Description: Closes an open file pointer

 Prototype: bool feof ( resource $handle )
 Description: Returns TRUE if the file pointer is at EOF or an error occurs 
   (including socket timeout); otherwise returns FALSE. 
*/

echo "*** Testing error conditions for fopen(), fclsoe() & feof() ***\n";
/* Arguments less than minimum no.of args */

// fopen ()
var_dump(fopen(__FILE__)); // one valid argument
var_dump(fopen());  // zero argument

// fclose()
$fp = fopen(__FILE__, "r");
fclose($fp);
var_dump( fclose($fp) ); // closed handle
var_dump( fclose(__FILE__) ); // invalid handle
var_dump( fclose() ); // zero argument 

//feof()
var_dump( feof($fp) );  // closed handle
var_dump( feof(__FILE__) );  // invalid handle
var_dump( feof() );  //zero argument

/* Arguments greater than maximum no.of ags */
var_dump(fopen(__FILE__, "r", TRUE, "www.example.com", 100));

$fp = fopen(__FILE__, "r");
var_dump( fclose($fp, "handle") );

var_dump( feof($fp, "handle"));
fclose($fp);

/* test invalid arguments : non-resources */
echo "-- Testing fopen(), fclose() & feof() with invalid arguments --\n";
$invalid_args = array (
  "string",
  10,
  10.5,
  true,
  array(1,2,3),
  new stdclass,
  NULL,
  ""
);

/* loop to test fclose with different invalid type of args */
for($loop_counter = 1; $loop_counter <= count($invalid_args); $loop_counter++) {
  echo "-- Iteration $loop_counter --\n";
  var_dump( fopen($invalid_args[$loop_counter - 1], "r") );
  var_dump( fclose($invalid_args[$loop_counter - 1]) );
  var_dump( feof($invalid_args[$loop_counter - 1]) );
}

?>
--EXPECTF--
*** Testing error conditions for fopen(), fclsoe() & feof() ***

Warning: fopen() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

Warning: fopen() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: Wrong parameter count for fclose() in %s on line %d
NULL

Warning: feof(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: Wrong parameter count for feof() in %s on line %d
NULL

Warning: fopen() expects at most 4 parameters, 5 given in %s on line %d
bool(false)

Warning: Wrong parameter count for fclose() in %s on line %d
NULL

Warning: Wrong parameter count for feof() in %s on line %d
NULL
-- Testing fopen(), fclose() & feof() with invalid arguments --
-- Iteration 1 --

Warning: fopen(string): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 2 --

Warning: fopen(10): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 3 --

Warning: fopen(10.5): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 4 --

Warning: fopen(1): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 5 --

Warning: fopen() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 6 --

Warning: fopen() expects parameter 1 to be string, object given in %s on line %d
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 7 --
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
-- Iteration 8 --
bool(false)

Warning: fclose(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: feof(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
