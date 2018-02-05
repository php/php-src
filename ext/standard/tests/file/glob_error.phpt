--TEST--
Test glob() function: error conditions
--FILE--
<?php
/* Prototype: array glob ( string $pattern [, int $flags] );
   Description: Find pathnames matching a pattern
*/

$file_path = dirname(__FILE__);

// temp dir created
mkdir("$file_path/glob_error");
// temp file created
$fp = fopen("$file_path/glob_error/wonder12345", "w");
fclose($fp);

echo "*** Testing glob() : error conditions ***\n";

echo "-- Testing glob() with unexpected no. of arguments --\n";
var_dump( glob() );  // args < expected
var_dump( glob(dirname(__FILE__)."/glob_error/wonder12345", GLOB_ERR, 3) );  // args > expected

echo "\n-- Testing glob() with invalid arguments --\n";
var_dump( glob(dirname(__FILE__)."/glob_error/wonder12345", '') );
var_dump( glob(dirname(__FILE__)."/glob_error/wonder12345", "string") );

echo "Done\n";
?>
--CLEAN--
<?php
// temp file deleted
unlink(dirname(__FILE__)."/glob_error/wonder12345");
// temp dir deleted
rmdir(dirname(__FILE__)."/glob_error");
?>
--EXPECTF--
*** Testing glob() : error conditions ***
-- Testing glob() with unexpected no. of arguments --

Warning: glob() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: glob() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Testing glob() with invalid arguments --

Warning: glob() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: glob() expects parameter 2 to be int, string given in %s on line %d
NULL
Done
