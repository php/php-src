--TEST--
Test disk_total_space() function : error conditions
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN' )
  die("skip Not Valid for Windows");
?>
--FILE--
<?php
/*
 *  Prototype: float disk_total_space( string $directory );
 *  Description: given a string containing a directory, this function
 *               will return the total number of bytes on the corresponding 
 *               filesystem or disk partition
 */

echo "*** Testing error conditions ***\n";
$file_path = dirname(__FILE__);
var_dump( disk_total_space() ); // Zero Arguments

var_dump( disk_total_space( $file_path, "extra argument") ); // More than valid number of arguments


var_dump( disk_total_space( $file_path."/dir1" )); // Invalid directory

$fh = fopen( $file_path."/disk_total_space.tmp", "w" );
fwrite( $fh, " Garbage data for the temporary file" );
var_dump( disk_total_space( $file_path."/disk_total_space.tmp" )); // file input instead of directory
fclose($fh);

echo"\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/disk_total_space.tmp");
?>
--EXPECTF--
*** Testing error conditions ***

Warning: disk_total_space() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: disk_total_space() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: disk_total_space(): No such file or directory in %s on line %d
bool(false)
float(%d)

--- Done ---
