--TEST--
Test disk_total_space() function : basic functionality
--FILE--
<?php
/*
 *  Prototype: float disk_total_space( string $directory );
 *  Description: given a string containing a directory, this 
 *               function will return the total number of bytes 
 *               on the corresponding filesyatem or disk partition.
 */

$file_path = dirname(__FILE__);

echo "*** Testing with existing directory ***\n";
var_dump( disk_total_space($file_path) );

echo "*** Testing with newly created directory ***\n";
mkdir($file_path."/disk_total_space");
var_dump( disk_total_space($file_path."/disk_total_space") );

$fh = fopen($file_path."/disk_total_space/disk_total_space.tmp", "w");
fwrite($fh, "Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data");

fclose($fh);

echo" \n Total Space after writing to a file\n";
var_dump( disk_total_space($file_path."/disk_total_space") );

echo"\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/disk_total_space/disk_total_space.tmp");
rmdir($file_path."/disk_total_space");
?>

--EXPECTF--
*** Testing with existing directory ***
float(%d)
*** Testing with newly created directory ***
float(%d)
 
 Total Space after writing to a file
float(%d)

--- Done ---
