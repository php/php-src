--TEST--
Test disk_free_space and its alias diskfreespace() functions : error conditions.
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN')
  die("skip Not valid on Windows");
?>
--FILE--
<?php
/*
 *  Prototype: float disk_free_space( string directory )
 *  Description: Given a string containing a directory, this function will 
 *               return the number of bytes available on the corresponding 
 *               filesystem or disk partition
 */

echo "*** Testing error conditions ***\n";
$file_path = dirname(__FILE__);
var_dump( disk_free_space() ); // Zero Arguments
var_dump( diskfreespace() );

var_dump( disk_free_space( $file_path, "extra argument") ); // More than valid number of arguments
var_dump( diskfreespace( $file_path, "extra argument") );


var_dump( disk_free_space( $file_path."/dir1" )); // Invalid directory
var_dump( diskfreespace( $file_path."/dir1" ));

$fh = fopen( $file_path."/disk_free_space.tmp", "w" );
fwrite( $fh, (binary)" Garbage data for the temporary file" );
var_dump( disk_free_space( $file_path."/disk_free_space.tmp" )); // file input instead of directory
var_dump( diskfreespace( $file_path."/disk_free_space.tmp" ));
fclose($fh);

echo"\n-- Done --";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/disk_free_space.tmp");

?>
--EXPECTF--
*** Testing error conditions ***

Warning: disk_free_space() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: diskfreespace() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: disk_free_space() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: diskfreespace() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: disk_free_space(): No such file or directory in %s on line %d
bool(false)

Warning: diskfreespace(): No such file or directory in %s on line %d
bool(false)
float(%d)
float(%d)

-- Done --

