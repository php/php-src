--TEST--
Test stat() functions: usage variations - effects of truncate()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php

/* test the effects of truncate() on stats of file */


$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp file and directory */

$filename = "$file_path/stat_variation8.tmp";
$file_handle = fopen($filename, "w");  // temp file
fclose($file_handle);


echo "\n*** Testing stat(): on file by truncating it to given size ***\n";

// create temp file
$file_handle = fopen($filename, "w");
fclose($file_handle);

clearstatcache(true, $filename);
$old_stat = stat($filename);
// clear the cache
sleep(1);

// opening file in r/w mode
$file_handle = fopen($filename, "r+");
var_dump( ftruncate($file_handle, 512) );  // truncate it
fclose($file_handle);

clearstatcache(true, $filename);
$new_stat = stat($filename);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stat
$affected_members = array(7, 9, 'size', 'mtime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, '!=') );

// clear the stat
clearstatcache(true, $filename);  // clear previous size value in cache

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/stat_variation8.tmp");
?>
--EXPECT--
*** Testing stat(): on file by truncating it to given size ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Done ***
