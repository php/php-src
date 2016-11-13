--TEST--
Test glob() function: error conditions
--FILE--
<?php
/* Prototype: array glob ( string $pattern [, int $flags] );
   Description: Find pathnames matching a pattern
*/

$file_path = __DIR__;

// temp dir created
mkdir("$file_path/glob_error");
// temp file created
$fp = fopen("$file_path/glob_error/wonder12345", "w");
fclose($fp);

echo "*** Testing glob() : error conditions ***\n";

echo "-- Testing glob() with unexpected no. of arguments --\n";
var_dump( glob() );  // args < expected
var_dump( glob(__DIR__."/glob_error/wonder12345", GLOB_ERR, 3) );  // args > expected

echo "\n-- Testing glob() with invalid arguments --\n";
var_dump( glob(__DIR__."/glob_error/wonder12345", '') );
var_dump( glob(__DIR__."/glob_error/wonder12345", "string") );

echo "Done\n";
?>
--CLEAN--
<?php
// temp file deleted
unlink(__DIR__."/glob_error/wonder12345");
// temp dir deleted
rmdir(__DIR__."/glob_error");
?>
--EXPECTF--
*** Testing glob() : error conditions ***
-- Testing glob() with unexpected no. of arguments --

Warning: glob() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: glob() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Testing glob() with invalid arguments --

Warning: glob() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: glob() expects parameter 2 to be integer, string given in %s on line %d
NULL
Done
