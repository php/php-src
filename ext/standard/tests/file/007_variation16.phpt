--TEST--
Test fopen and fclose() functions - usage variations - "x+t" mode 
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

/* Test fopen() and fclose(): Opening the file in "x+t" mode,
   checking for the file creation, write & read operations,
   checking for the file pointer position,
   checking for the warning msg when trying to open an existing file in "x+t" mode,  
   and fclose function
*/
$file_path = dirname(__FILE__);
$string = "abcdefghij\nmnopqrst\tuvwxyz\n0123456789";
$file = $file_path."/007_variation16.tmp";

echo "*** Test fopen() & fclose() functions:  with 'x+t' mode ***\n";
$file_handle = fopen($file, "x+t");  //opening the non-existing file in "x+t" mode, file will be created
var_dump($file_handle);  //Check for the content of handle
var_dump( get_resource_type($file_handle) );  //Check for the type of resource
var_dump( ftell($file_handle) );  //Initial file pointer position, expected at the begining of the file
var_dump( fwrite($file_handle, $string) );  //Check for write operation; passes; expected:size of the $string
var_dump( ftell($file_handle) );  //File pointer position after write operation, expected at the end of the file
rewind($file_handle);
var_dump( fread($file_handle, 100) );  //Check for read operation; passes; expected: content of the file
var_dump( ftell($file_handle) );  //File pointer position after read operation, expected at the end of the file
var_dump( fclose($file_handle) );  //Check for close operation on the file handle
var_dump( get_resource_type($file_handle) );  //Check whether resource is lost after close operation
$file_handle = fopen($file, "x+t");  //Opening the existing data file in "x+t" mode to check for the warning message
echo "*** Done ***\n"; 
--CLEAN--
<?php
unlink(dirname(__FILE__)."/007_variation16.tmp");
?>
--EXPECTF--
*** Test fopen() & fclose() functions:  with 'x+t' mode ***
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

Warning: fopen(%s): failed to open stream: File exists in %s on line %d
*** Done ***
