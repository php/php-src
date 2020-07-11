--TEST--
Test lstat() and stat() functions: usage variations - effects of is_dir()
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
/* test the effects of is_dir() on stats of a dir */

$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp file, link and directory */
$dirname = "$file_path/lstat_stat_variation10";
mkdir($dirname);  // temp dir

// is_dir() on a directory
echo "*** Testing stat() on directory after using is_dir() on it ***\n";
$old_stat = stat($dirname);
// clear the cache
clearstatcache();
sleep(1);
var_dump( is_dir($dirname) );
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// activity from background processes may unexpectedly update the atime
// so don't include "atime" (or 8, which also means atime)
$compare_keys = array(0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12,
                      "dev", "ino", "mode", "nlink", "uid", "gid",
                      "rdev", "size", "mtime", "ctime",
                      "blksize", "blocks");
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $compare_keys) );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir("$file_path/lstat_stat_variation10");
?>
--EXPECT--
*** Testing stat() on directory after using is_dir() on it ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
