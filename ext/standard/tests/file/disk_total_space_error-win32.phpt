--TEST--
Test disk_total_space() function : error conditions
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip Valid only for Windows");
?>
--CONFLICTS--
disk_total_space
--FILE--
<?php
echo "*** Testing error conditions ***\n";
$file_path = __DIR__;

var_dump( disk_total_space( $file_path."/dir1" )); // Invalid directory

$fh = fopen( $file_path."/disk_total_space.tmp", "w" );
fwrite( $fh, " Garbage data for the temporary file" );
var_dump( disk_total_space( $file_path."/disk_total_space.tmp" )); // file input instead of directory
fclose($fh);

echo"\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/disk_total_space.tmp");
?>
--EXPECTF--
*** Testing error conditions ***

Warning: disk_total_space(): The system cannot find the path specified in %s on line %d
bool(false)

Warning: disk_total_space(): The directory name is invalid in %s on line %d
bool(false)

--- Done ---
