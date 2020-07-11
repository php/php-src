--TEST--
Test stat() functions: usage variations - changing permissions of dir/file
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php

/* test the effects on the stats of dir/file for changing permissions of dir/file */


$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp file and directory */
$dirname = "$file_path/stat_variation6";
mkdir($dirname);  // temp dir

$filename = "$file_path/stat_variation6.tmp";
$file_handle = fopen($filename, "w");  // temp file
fclose($file_handle);


// checking stat() on file
echo "\n*** Testing stat() on file with miscellaneous file permission and content ***\n";
clearstatcache();
$old_stat = stat($filename);
sleep(1);
var_dump( chmod($filename, 0777) );
// clear the stat
clearstatcache();
$new_stat = stat($filename);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array( 10, 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "==") );
// clear the stat
clearstatcache();  // clear statement cache

// checking stat() on directory
echo "\n*** Testing stat() on directory with miscellaneous file permission ***\n";
$old_stat = stat($dirname);
sleep(1);
var_dump( chmod($dirname, 0777) );
// clear the stat
clearstatcache();
$new_stat = stat($dirname);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array( 10, 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "==") );
// clear the stat
clearstatcache();  // clear statement cache


echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/stat_variation6.tmp");
rmdir("$file_path/stat_variation6");
?>
--EXPECT--
*** Testing stat() on file with miscellaneous file permission and content ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Testing stat() on directory with miscellaneous file permission ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Done ***
