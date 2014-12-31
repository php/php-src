--TEST--
Test file() function : error conditions
--FILE--
<?php
/* 
   Prototype: array file ( string filename [,int use-include_path [,resource context]] );
   Description: Reads entire file into an array
                Returns the  file in an array
*/
$file_path = dirname(__FILE__);
echo "\n*** Testing error conditions ***";
$file_handle = fopen($file_path."/file.tmp", "w");
var_dump( file() );  // Zero No. of args

$filename = $file_path."/file.tmp";
var_dump( file($filename, $filename, $filename, $filename) );  // more than expected number of arguments

var_dump( file($filename, "INCORRECT_FLAG", NULL) );  //  Incorrect flag
var_dump( file($filename, 10, NULL) );  //  Incorrect flag

var_dump( file("temp.tmp") );  // non existing filename 
fclose($file_handle);

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/file.tmp");
?>
--EXPECTF--
*** Testing error conditions ***
Warning: file() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: file() expects at most 3 parameters, 4 given in %s on line %d
NULL

Warning: file() expects parameter 2 to be integer, string given in %s on line %d
NULL
array(0) {
}

Warning: file(temp.tmp): failed to open stream: No such file or directory in %s on line %d
bool(false)

--- Done ---

