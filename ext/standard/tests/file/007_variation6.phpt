--TEST--
Test fopen and fclose() functions - usage variations - "a+" mode
--FILE--
<?php

/* Test fopen() and fclose(): Opening the file in "a+" mode,
   checking for the file creation, write & read operations,
   checking for the file pointer position,
   and fclose function
*/
$file_path = __DIR__;
require($file_path."/file.inc");

create_files($file_path, 1, "text_with_new_line", 0755, 20, "w", "007_variation", 6, "bytes");
$file = $file_path."/007_variation6.tmp";
$string = "abcdefghij\nmnopqrst\tuvwxyz\n0123456789";

echo "*** Test fopen() & fclose() functions:  with 'a+' mode ***\n";
$file_handle = fopen($file, "a+");  //opening the file "a+" mode
var_dump($file_handle);  //Check for the content of handle
var_dump( get_resource_type($file_handle) );  //Check for the type of resource
var_dump( fwrite($file_handle, $string) );  //Check for write operation; passes; expected:size of the $string
rewind($file_handle);
var_dump( fread($file_handle, 100) );  //Check for read operation; passes; expected: content of file
var_dump( ftell($file_handle) );  //File pointer position after read operation, expected at the end of the file
var_dump( fclose($file_handle) );  //Check for close operation on the file handle
var_dump( get_resource_type($file_handle) );  //Check whether resource is lost after close operation

unlink($file);  //Deleting the file
fclose( fopen($file, "a+") );  //Opening the non-existing file in "a+" mode, which will be created
var_dump( file_exists($file) );  //Check for the existence of file
echo "*** Done ***\n";
--CLEAN--
<?php
unlink(__DIR__."/007_variation6.tmp");
?>
--EXPECTF--
*** Test fopen() & fclose() functions:  with 'a+' mode ***
resource(%d) of type (stream)
string(6) "stream"
int(37)
string(57) "line
line of text
liabcdefghij
mnopqrst	uvwxyz
0123456789"
int(57)
bool(true)
string(7) "Unknown"
bool(true)
*** Done ***
