--TEST--
Test disk_free_space and its alias diskfreespace() functions : error conditions
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip Valid only for Windows");
?>
--FILE--
<?php
echo "*** Testing error conditions ***\n";
$file_path = __DIR__;

var_dump( disk_free_space( $file_path."/dir1" )); // Invalid directory
var_dump( diskfreespace( $file_path."/dir1" ));

$fh = fopen( $file_path."/disk_free_space.tmp", "w" );
fwrite( $fh, " Garbage data for the temporary file" );
var_dump( disk_free_space( $file_path."/disk_free_space.tmp" )); // file input instead of directory
var_dump( diskfreespace( $file_path."/disk_free_space.tmp" ));
fclose($fh);

echo"\n-- Done --";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/disk_free_space.tmp");

?>
--EXPECTF--
*** Testing error conditions ***

Warning: disk_free_space(): The system cannot find the path specified in %s on line %d
bool(false)

Warning: diskfreespace(): The system cannot find the path specified in %s on line %d
bool(false)

Warning: disk_free_space(): The directory name is invalid in %s on line %d
bool(false)

Warning: diskfreespace(): The directory name is invalid in %s on line %d
bool(false)

-- Done --
