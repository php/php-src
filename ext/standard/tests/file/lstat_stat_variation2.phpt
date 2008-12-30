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

$file_path = dirname(__FILE__);
require("file.inc");

/* create temp directory */
@rmdir("$file_path/lstat_stat_variation2/");  //ensure that dir doesn't exists 
mkdir("$file_path/lstat_stat_variation2/");  // temp dir

// renaming a directory and check stat
echo "*** Testing stat() for directory after being renamed ***\n";
$old_dirname = "$file_path/lstat_stat_variation2";
$new_dirname = "$file_path/lstat_stat_variation2a";
$old_stat = stat($old_dirname);
clearstatcache();
var_dump( rename($old_dirname, $new_dirname) );
$new_stat = stat($new_dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );
// clear the cache
clearstatcache();

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir("$file_path/lstat_stat_variation2a");
?>
--EXPECT--
*** Testing stat() for directory after being renamed ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
