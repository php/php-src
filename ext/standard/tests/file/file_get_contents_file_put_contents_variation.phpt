--TEST--
Test file_get_contents() and file_put_contents() functions : usage variations

--FILE--
<?php
/*  Prototype: string file_get_contents( string $filename[, bool $use_include_path[, 
 *                                       resource $context[, int $offset[, int $maxlen]]]] )
 *  Description: Reads entire file into a string
 */

/*  Prototype: int file_put_contents( string $filename, mixed $data[, int $flags[, resource $context]] )
 *  Description: Write a string to a file
 */

$file_path = dirname(__FILE__);
include($file_path."/file.inc");

echo "*** Testing with variations in the arguments values ***\n";

$buffer_types = array("text", "numeric", "text_with_new_line", "alphanumeric");

foreach( $buffer_types as $type) {
  fill_buffer($buffer, $type, 100);
  file_put_contents( $file_path."/file_get_contents.tmp", $buffer, FILE_BINARY );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 0) );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 1) );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 0, NULL, 5) );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 1, NULL, 5) );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 0, NULL, 5, 20) );
  var_dump( file_get_contents($file_path."/file_get_contents.tmp", 1, NULL, 5, 20) );
}

echo "*** Testing with variation in use_include_path argument ***\n";
$dir = "file_get_contents";
mkdir($file_path."/".$dir);
$filename = $file_path."/".$dir."/"."file_get_contents1.tmp";

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

$file_path = dirname(__FILE__);
unlink($file_path."/file_get_contents.tmp");
unlink($file_path."/file_get_contents/file_get_contents1.tmp");
rmdir($file_path."/file_get_contents");

?>
--EXPECTF--
*** Testing with variations in the arguments values ***
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(20) "text text text text "
string(20) "text text text text "
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(20) "22222222222222222222"
string(20) "22222222222222222222"
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(20) "line of text
line
li"
string(20) "line of text
line
li"
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
*** Testing with variation in use_include_path argument ***
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(20) "text text text text "
string(20) "text text text text "
--- Done ---

--UEXPECTF--
*** Testing with variations in the arguments values ***

Notice: file_put_contents(): 100 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(20) "text text text text "
string(20) "text text text text "

Notice: file_put_contents(): 100 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(20) "22222222222222222222"
string(20) "22222222222222222222"

Notice: file_put_contents(): 100 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(20) "line of text
line
li"
string(20) "line of text
line
li"

Notice: file_put_contents(): 100 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
*** Testing with variation in use_include_path argument ***

Notice: file_put_contents(): 100 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: file_put_contents(): 19 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: file_put_contents(): 19 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(240) "text text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(235) "text text text text text text text text text text text text text text text text text text text 22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222221  Data1 in an array2  Data2 in an array"
string(20) "text text text text "
string(20) "text text text text "
--- Done ---
