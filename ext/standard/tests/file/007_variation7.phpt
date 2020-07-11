--TEST--
Test fopen and fclose() functions - usage variations - "x" mode
--FILE--
<?php

/* Test fopen() and fclose(): Opening the file in "x" mode,
   checking for the file creation, write & read operations,
   checking for the file pointer position,
   checking for the warning msg when trying to open an existing file in "x" mode,
   and fclose function
*/
$file_path = __DIR__;
$string = "abcdefghij\nmnopqrst\tuvwxyz\n0123456789";
$file = $file_path."/007_variation7.tmp";

echo "*** Test fopen() & fclose() functions:  with 'x' mode ***\n";
$file_handle = fopen($file, "x");  //opening the non-existing file in "x" mode, file will be created
var_dump($file_handle);  //Check for the content of handle
var_dump( get_resource_type($file_handle) );  //Check for the type of resource
var_dump( ftell($file_handle) );  //Initial file pointer position, expected at the beginning of the file
var_dump( fwrite($file_handle, $string) );  //Check for write operation; passes; expected:size of the $string
var_dump( ftell($file_handle) );  //File pointer position after write operation, expected at the end of the file
rewind($file_handle);
var_dump( fread($file_handle, 100) );  //Check for read operation; fails; expected: false
var_dump( ftell($file_handle) );  //File pointer position after read operation, expected at the beginning of the file
var_dump( fclose($file_handle) );  //Check for close operation on the file handle
var_dump( get_resource_type($file_handle) );  //Check whether resource is lost after close operation
$file_handle = fopen($file, "x");  //Opening the existing data file in 'x' mode to check for the warning message
echo "*** Done ***\n";
--CLEAN--
<?php
unlink(__DIR__."/007_variation7.tmp");
?>
--EXPECTF--
*** Test fopen() & fclose() functions:  with 'x' mode ***
resource(%d) of type (stream)
string(6) "stream"
int(0)
int(37)
int(37)

Notice: fread(): Read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(true)
string(7) "Unknown"

Warning: fopen(%s): Failed to open stream: File exists in %s on line %d
*** Done ***
