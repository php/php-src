--TEST--
Test lstat() and stat() functions: usage variations - effects of touch() on dir
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/* test the effects of touch() on stats of dir */

$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp directory */

$dir_name = "$file_path/lstat_stat_variation5";
@rmdir($dir_name);  //ensure that dir doesn't exists
mkdir($dir_name);  // temp dir

// touch a directory and check stat, there should be difference in atime
echo "*** Testing stat() for directory after using touch() on the directory ***\n";
$old_stat = stat($dir_name);
// clear the cache
clearstatcache();
sleep(1);
var_dump( touch($dir_name) );
$new_stat = stat($dir_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stat
$affected_members = array(8, 9, 10, 'atime', 'mtime', 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "<") );
// clear the cache
clearstatcache();

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir("$file_path/lstat_stat_variation5");
?>
--EXPECT--
*** Testing stat() for directory after using touch() on the directory ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
