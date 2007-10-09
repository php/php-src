--TEST--
Test fopen and fclose() functions - usage variations - "r+t" mode 
--FILE--
<?php
/*
 fopen() function:
 Prototype: resource fopen(string $filename, string $mode
                            [, bool $use_include_path [, resource $context]] );
 Description: Opens file or URL.
*/
/*
 fclose() function:
 Prototype: bool fclose ( resource $handle );
 Description: Closes an open file pointer
*/

/* Test fopen() and fclose(): Opening the file in "r+t" mode,
   checking for the file creation, write & read operations,
   checking for the file pointer position,
   and fclose function
*/
$file_path = dirname(__FILE__);
require($file_path."/file.inc");

create_files($file_path, 1, "text_with_new_line", 0755, 20, "w", "007_variation", 10, "bytes");
$file = $file_path."/007_variation10.tmp";
$string = "abcdefghij\nmnopqrst\tuvwxyz\n0123456789";

echo "*** Test fopen() & fclose() functions:  with 'r+t' mode ***\n";
$file_handle = fopen($file, "r+t");  //opening the file in "r+t" mode
var_dump($file_handle);  //Check for the content of handle
var_dump( get_resource_type($file_handle) );  //Check for the type of resource
var_dump( ftell($file_handle) );  //Initial file pointer position, expected at the begining of the file
var_dump( fread($file_handle, 100) );  //Check for read operation
var_dump( ftell($file_handle) );  //File pointer position after read operation, expected at the end of the file
var_dump( fwrite($file_handle, $string) );  //Check for write operation; passes; expected:size of the $string
var_dump( ftell($file_handle) );  //File pointer position after write operation, expected at the end of the file
var_dump( fclose($file_handle) );  //Check for close operation on the file handle
var_dump( get_resource_type($file_handle) );  //Check whether resource is lost after close operation
echo "*** Done ***\n"; 
--CLEAN--
<?php
unlink(dirname(__FILE__)."/007_variation10.tmp");
?>
--EXPECTF--
*** Test fopen() & fclose() functions:  with 'r+t' mode ***
resource(%d) of type (stream)
string(6) "stream"
int(0)
string(20) "line
line of text
li"
int(20)
int(37)
int(57)
bool(true)
string(7) "Unknown"
*** Done ***
