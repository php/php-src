--TEST--
Test lstat() and stat() functions: usage variations - effects of is_dir()
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

/* test the effects of is_dir() on stats of a dir */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file, link and directory */
$dirname = "$file_path/lstat_stat_variation10";
mkdir($dirname);  // temp dir

// is_dir() on a directory
echo "*** Testing stat() on directory after using is_dir() on it ***\n";
$old_stat = stat($dirname);
// clear the cache
clearstatcache();
sleep(2);
var_dump( is_dir($dirname) );
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir("$file_path/lstat_stat_variation10");
?>
--EXPECTF--
*** Testing stat() on directory after using is_dir() on it ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
