--TEST--
Test disk_free_space and its alias diskfreespace() functions : basic functionality
--INI--
memory_limit=32M
--FILE--
<?php
/*
 *  Prototype: float disk_free_space( string directory )
 *  Description: Given a string containing a directory, this function 
 *  will return the number of bytes available on the corresponding 
 *  filesystem or disk partition
 */

$file_path = dirname(__FILE__);
include($file_path."/file.inc");

echo "*** Testing with existing directory ***\n";
var_dump( disk_free_space($file_path) ); 
var_dump( diskfreespace($file_path) ); 
$dir = "/disk_free_space";

echo "*** Testing with newly created directory ***\n";
mkdir($file_path.$dir);
echo" \n Free Space before writing to a file\n";
$space1 =  disk_free_space($file_path.$dir); 
var_dump($space1); 

fill_buffer($buffer, "text", 3000000);
file_put_contents($file_path.$dir."/disk_free_space.tmp", $buffer);

echo "\n Free Space after writing to a file\n";
$space2 =  disk_free_space($file_path.$dir); 
var_dump( $space2 ); 

if( $space1 > $space2 )
  echo "\n Free Space Value Is Correct\n";
else
  echo "\n Free Space Value Is Incorrect\n";

echo"\n-- Done --";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir = "/disk_free_space";
unlink($file_path.$dir."/disk_free_space.tmp");
rmdir($file_path.$dir);
?>

--EXPECTF--
*** Testing with existing directory ***
float(%f)
float(%f)
*** Testing with newly created directory ***
 
 Free Space before writing to a file
float(%f)

 Free Space after writing to a file
float(%f)

 Free Space Value Is Correct

-- Done --
