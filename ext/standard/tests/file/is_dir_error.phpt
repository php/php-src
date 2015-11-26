--TEST--
Test is_dir() function: error conditions
--FILE--
<?php
/* Prototype: bool is_dir ( string $filename );
 *  Description: Tells whether the filename is a regular file
 *               Returns TRUE if the filename exists and is a regular file
 */

echo "*** Testing is_dir() error conditions ***";
var_dump( is_dir() );  // Zero No. of args

$dir_name = dirname(__FILE__)."/is_dir_error";
mkdir($dir_name);
var_dump( is_dir($dir_name, "is_dir_error1") ); // args > expected no.of args

/* Non-existing dir */
var_dump( is_dir("/no/such/dir") );

echo "*** Done ***";
?>

--CLEAN--
<?php
rmdir(dirname(__FILE__)."/is_dir_error");
?>
--EXPECTF--
*** Testing is_dir() error conditions ***
Warning: is_dir() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_dir() expects exactly 1 parameter, 2 given in %s on line %d
NULL
bool(false)
*** Done ***
