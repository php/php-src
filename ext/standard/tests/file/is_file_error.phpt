--TEST--
Test is_file() function: error conditions
--FILE--
<?php
/* Prototype: bool is_file ( string $filename );
   Description: Tells whether the filename is a regular file
                Returns TRUE if the filename exists and is a regular file
*/

echo "\n*** Testing is_file() error conditions ***";
$file_path = dirname(__FILE__);
var_dump( is_file() );  // Zero No. of args

/* no.of args > expected no.of args */
$file_handle = fopen($file_path."/is_file_error.tmp", "w");
var_dump( is_file( $file_path."/is_file_error.tmp", $file_path."/is_file_error1.tmp") );

/* Non-existing file */
var_dump( is_file($file_path."/is_file_error1.tmp") );

/* Passing resource as an argument */
var_dump( is_file($file_handle) );

fclose($file_handle);

echo "\n*** Done ***";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/is_file_error.tmp");
?>
--EXPECTF--
*** Testing is_file() error conditions ***
Warning: Wrong parameter count for is_file() in %s on line %d
NULL

Warning: Wrong parameter count for is_file() in %s on line %d
NULL
bool(false)
bool(false)

*** Done ***
