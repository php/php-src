--TEST--
Test file_get_contents() and file_put_contents() functions : usage variations - use_include_path
--FILE--
<?php

/* Testing variation using use_include_path argument */
$file_path = __DIR__;
include($file_path."/file.inc");

echo "*** Testing with variation in use_include_path argument ***\n";
$dir = "file_get_contents_variation2";
mkdir($file_path."/".$dir);
$filename = $file_path."/".$dir."/"."file_get_contents_variation2.tmp";

ini_set( 'include_path',$file_path."/".$dir );

$data_array = array( 1, "  Data1 in an array", 2, "  Data2 in an array" );
fill_buffer( $buffer, "text", 100);
file_put_contents( $filename, $buffer );
fill_buffer( $buffer, "numeric", 100);
file_put_contents( $filename, $buffer, FILE_APPEND, NULL );
file_put_contents( $filename, $data_array, FILE_APPEND, NULL );
var_dump( file_get_contents($filename, 0) );
var_dump( file_get_contents($filename, 1) );
var_dump( file_get_contents($filename, 0, NULL, 5) );
var_dump( file_get_contents($filename, 1, NULL, 5) );
var_dump( file_get_contents($filename, 0, NULL, 5, 20) );
var_dump( file_get_contents($filename, 1, NULL, 5, 20) );

echo "--- Done ---";
?>
--CLEAN--
<?php
//Deleting the temporary files and directory used in the testcase

$file_path = __DIR__;
unlink($file_path."/file_get_contents_variation2/file_get_contents_variation2.tmp");
rmdir($file_path."/file_get_contents_variation2");
?>
--EXPECT--
*** Testing with variation in use_include_path argument ***
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(20) "text text text text "
string(20) "text text text text "
--- Done ---
