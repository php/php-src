--TEST--
Test fileowner() function: basic functionality
--FILE--
<?php
/* Prototype: int fileowner ( string $filename )
 * Description: Returns the user ID of the owner of the file, or
 *              FALSE in case of an error.
 */

echo "*** Testing fileowner(): basic functionality ***\n";

echo "-- Testing with the file or directory created by owner --\n";
var_dump( fileowner(__FILE__) );
var_dump( fileowner(".") );
var_dump( fileowner("./..") );

/* Newly created files and dirs */
$file_path = dirname(__FILE__);
$file_name = $file_path."/fileowner_basic.tmp";
$file_handle = fopen($file_name, "w");
$string = "Hello, world\n1234\n123Hello";
fwrite($file_handle, $string);
var_dump( fileowner($file_name) );
fclose($file_handle);

$dir_name = $file_path."/fileowner_basic";
mkdir($dir_name);
var_dump( fileowner($dir_name) );

echo "*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$file_name = $file_path."/fileowner_basic.tmp";
$dir_name = $file_path."/fileowner_basic";
unlink($file_name);
rmdir($dir_name);
?>
--EXPECTF--
*** Testing fileowner(): basic functionality ***
-- Testing with the file or directory created by owner --
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
*** Done ***
