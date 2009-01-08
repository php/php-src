--TEST--
Test disk_free_space and its alias diskfreespace() functions : basic functionality
--INI--
memory_limit=32M
--FILE--
<?php
/*
 *  Prototype: float disk_free_space( string directory )
 *  Description: Given a string containing a directory, this function 
 *               will return the number of bytes available on the corresponding 
 *               filesystem or disk partition
 */

$file_path = dirname(__FILE__);

echo "*** Testing with existing directory ***\n";
var_dump( disk_free_space($file_path) ); 
var_dump( diskfreespace($file_path) ); 

echo "*** Testing with newly created directory ***\n";
$dir = "/disk_free_space";
mkdir($file_path.$dir);
echo" \n Free Space before writing to a file\n";
$space1 =  disk_free_space($file_path.$dir); 
var_dump( $space1 ); 

$fh = fopen($file_path.$dir."/disk_free_space.tmp", "a");
$data = str_repeat("x", 4096);
fwrite($fh, (binary)$data);
fclose($fh);

echo "\n Free Space after writing to a file\n";
$space2 =  disk_free_space($file_path.$dir); 
var_dump( $space2 ); 

if( $space1 > $space2 )
  echo "\n Free Space Value Is Correct\n";
else
  echo "\n Free Space Value Is Incorrect\n";

echo "*** Testing with Binary Input ***\n";
var_dump( disk_free_space(b"$file_path") ); 

echo"\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/disk_free_space/disk_free_space.tmp");
rmdir($file_path."/disk_free_space");
?>

--EXPECTF--
*** Testing with existing directory ***
float(%d)
float(%d)
*** Testing with newly created directory ***
 
 Free Space before writing to a file
float(%d)

 Free Space after writing to a file
float(%d)

 Free Space Value Is Correct
*** Testing with Binary Input ***
float(%d)

--- Done ---
