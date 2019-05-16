--TEST--
Test lstat() and stat() functions: usage variations - effects of rename() on dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

/* test the effects of rename() on stats of dir */

$file_path = __DIR__;
require("file.inc");

/* create temp directory */
mkdir("$file_path/lstat_stat_variation1/");  // temp dir

// renaming a directory and check stat
echo "*** Testing stat() for directory after being renamed ***\n";
$old_dirname = "$file_path/lstat_stat_variation1";
$new_dirname = "$file_path/lstat_stat_variation1a";
$old_stat = stat($old_dirname);
clearstatcache();
var_dump( rename($old_dirname, $new_dirname) );
$new_stat = stat($new_dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats - all except ctime
$keys_to_compare = array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12,
                       "dev", "ino", "mode", "nlink", "uid", "gid",
                       "rdev", "size", "atime", "mtime", "blksize", "blocks");
var_dump( compare_stats($old_stat, $new_stat, $keys_to_compare) );
// clear the cache
clearstatcache();

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir("$file_path/lstat_stat_variation1a");
?>
--EXPECT--
*** Testing stat() for directory after being renamed ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
