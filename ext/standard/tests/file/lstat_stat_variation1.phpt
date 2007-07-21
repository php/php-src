--TEST--
Test lstat() and stat() functions: usage variations - effects of rename()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. lstat() not available on Windows');
}
?>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

/* test the effects of rename() on stats of dir/file/link */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";

/* create temp file, link and directory */
mkdir("$file_path/lstat_stat_variation1/");  // temp dir
$fp = fopen("$file_path/lstat_stat_variation1.tmp", "w");  // temp file
fclose($fp);

// temp link
symlink("$file_path/lstat_stat_variation1.tmp", "$file_path/lstat_stat_variation_link1.tmp");

echo "*** Checking lstat() and stat() on file, link and directory ater renaming them ***\n";

// renaming a file
echo "-- Testing stat() for files after being renamed --\n";
$file_path = dirname(__FILE__);
$old_filename = "$file_path/lstat_stat_variation1.tmp";
$new_filename = "$file_path/lstat_stat_variation1a.tmp";
$old_stat = stat($old_filename);
clearstatcache();
var_dump( rename($old_filename, $new_filename) );
$new_stat = stat($new_filename);

// compare the self stat 
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats 
var_dump( compare_stats($old_stat, $old_stat, $all_stat_keys) );
// clear the cache
clearstatcache();

// renaming a directory
echo "-- Testing stat() for directory after being renamed --\n";
$old_dirname = "$file_path/lstat_stat_variation1";
$new_dirname = "$file_path/lstat_stat_variation1a";
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

// renaming a link
echo "-- Testing lstat() for link after being renamed --\n";
$old_linkname = "$file_path/lstat_stat_variation_link1.tmp";
$new_linkname = "$file_path/lstat_stat_variation_link1a.tmp";
$old_stat = lstat($old_linkname);
clearstatcache();
var_dump( rename($old_linkname, $new_linkname) );
$new_stat = lstat($new_linkname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation1a.tmp");
rmdir("$file_path/lstat_stat_variation1a");
unlink("$file_path/lstat_stat_variation_link1a.tmp");
?>
--EXPECTF--
*** Checking lstat() and stat() on file, link and directory ater renaming them ***
-- Testing stat() for files after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing stat() for directory after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing lstat() for link after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
--UEXPECTF--
*** Checking lstat() and stat() on file, link and directory ater renaming them ***
-- Testing stat() for files after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing stat() for directory after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing lstat() for link after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
