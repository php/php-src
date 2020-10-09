--TEST--
Test rename() function: basic functionality
--FILE--
<?php
echo "*** Testing rename() on non-existing file ***\n";
$file_path = __DIR__;
require "$file_path/file.inc";

$src_name = "$file_path/rename_basic.tmp";
$dest_name = "$file_path/rename_basic_new.tmp";

// create the file
$fp = fopen($src_name, "w");
$old_stat = stat($src_name);
fclose($fp);

var_dump( rename($src_name, $dest_name) ); // expecting true
var_dump( file_exists($src_name) ); // expecting false
var_dump( file_exists($dest_name) ); // expecting true

$new_stat = stat("$file_path/rename_basic_new.tmp");

// checking statistics of old and renamed file - both should be same except ctime
$keys_to_compare = array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12,
                       "dev", "ino", "mode", "nlink", "uid", "gid",
                       "rdev", "size", "atime", "mtime", "blksize", "blocks");
var_dump( compare_stats($old_stat, $new_stat, $keys_to_compare) );

?>
--CLEAN--
<?php
unlink(__DIR__."/rename_basic_new.tmp");
?>
--EXPECT--
*** Testing rename() on non-existing file ***
bool(true)
bool(false)
bool(true)
bool(true)
