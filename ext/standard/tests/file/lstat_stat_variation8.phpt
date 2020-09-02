--TEST--
Test lstat() and stat() functions: usage variations - creating file/subdir
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
$file_path = __DIR__;
require "$file_path/file.inc";

/* test the effects on stats with creating file/subdir in a dir
*/

/* create temp file */
mkdir("$file_path/lstat_stat_variation8/");  // temp dir

// creating and deleting subdir and files in the dir
echo "*** Testing stat() on dir after subdir and file is created in it ***\n";
$dirname = "$file_path/lstat_stat_variation8";
$old_stat = stat($dirname);
clearstatcache();
sleep(1);
mkdir("$dirname/lstat_stat_variation8_subdir");
$file_handle = fopen("$dirname/lstat_stat_variation8a.tmp", "w");
fclose($file_handle);
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array(9, 10, 'mtime', 'ctime');
clearstatcache();
var_dump(compare_stats($old_stat, $new_stat, $affected_members, "<"));

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation8/lstat_stat_variation8a.tmp");
rmdir("$file_path/lstat_stat_variation8/lstat_stat_variation8_subdir/");
rmdir("$file_path/lstat_stat_variation8");
?>
--EXPECT--
*** Testing stat() on dir after subdir and file is created in it ***
bool(true)
bool(true)
bool(true)

--- Done ---
