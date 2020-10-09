--TEST--
Test fopen and fclose() functions - usage variations - "w+t" mode
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die('skip Run only on Windows');
?>
--FILE--
<?php

/* Test fopen() and fclose(): Opening the file in "w+t" mode,
   checking for the file creation, write & read operations,
   checking for the file pointer position,
   checking for the file truncation when trying to open an existing file in "w+t" mode,
   and fclose function
*/
$file_path = __DIR__;
require($file_path."/file.inc");

create_files($file_path, 1, "text_with_new_line", 0755, 20, "w", "007_variation", 12, "bytes");
$file = $file_path."/007_variation12.tmp";
$string = "abcdefghij\nmnopqrst\tuvwxyz\n0123456789";

echo "*** Test fopen() & fclose() functions:  with 'w+t' mode ***\n";
$file_handle = fopen($file, "w+t");  //opening the file "w+t" mode
var_dump($file_handle);  //Check for the content of handle
var_dump( get_resource_type($file_handle) );  //Check for the type of resource
var_dump( ftell($file_handle) );  //Initial file pointer position, expected at the beginning of the file
var_dump( fwrite($file_handle, $string) );  //Check for write operation; passes; expected:size of the $string
var_dump( ftell($file_handle) );  //File pointer position after write operation, expected at the end of the file
rewind($file_handle);
var_dump( fread($file_handle, 100) );  //Check for read operation; passes; expected: content of file
var_dump( ftell($file_handle) );  //File pointer position after read operation, expected at the end of the file
var_dump( fclose($file_handle) );  //Check for close operation on the file handle
var_dump( get_resource_type($file_handle) );  //Check whether resource is lost after close operation

var_dump( filesize($file) );  //Check for size of existing data file before opening the file in "w+t" mode again, expected: size of content
clearstatcache();
fclose( fopen($file, "w+t") );  //Opening the existing data file again in "w+t" mode
var_dump( filesize($file) );  //Check for size of existing data file after opening the file in "w+t" mode again, expected: 0 bytes
clearstatcache();

unlink($file);  //Deleting the file
fclose( fopen($file, "w+t") );  //Opening the non-existing file in "w+t" mode, which will be created
var_dump( file_exists($file) );  //Check for the existence of file
echo "*** Done ***\n";
--CLEAN--
<?php
unlink(__DIR__."/007_variation12.tmp");
?>
--EXPECTF--
*** Test fopen() & fclose() functions:  with 'w+t' mode ***
resource(%d) of type (stream)
string(6) "stream"
int(0)
int(37)
int(37)
string(37) "abcdefghij
mnopqrst	uvwxyz
0123456789"
int(37)
bool(true)
string(7) "Unknown"
int(39)
int(0)
bool(true)
*** Done ***
