--TEST--
Test disk_total_space() function : basic functionality
--CONFLICTS--
disk_total_space
--FILE--
<?php
$file_path = __DIR__;

echo "*** Testing with normal directory ***\n";
var_dump( disk_total_space($file_path) );

echo "*** Testing with newly created directory ***\n";
$dir = "/disk_total_space";

mkdir($file_path.$dir);
var_dump( disk_total_space($file_path.$dir) );
$fh = fopen($file_path.$dir."/disk_total_space.tmp", "w");
fwrite($fh, "Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data Garbage Data");

fclose($fh);

echo"\nTotal Space after writing to a file\n";
var_dump( disk_total_space($file_path.$dir) );

echo"\n-- Done --";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/disk_total_space/disk_total_space.tmp");
rmdir($file_path."/disk_total_space");
?>
--EXPECTF--
*** Testing with normal directory ***
float(%f)
*** Testing with newly created directory ***
float(%f)

Total Space after writing to a file
float(%f)

-- Done --
