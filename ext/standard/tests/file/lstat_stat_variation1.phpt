--TEST--
Test lstat() and stat() functions: usage variations - effects of rename() on file
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

/* test the effects of rename() on stats of file */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";

/* create temp file */
$fp = fopen("$file_path/lstat_stat_variation1.tmp", "w");  // temp file
fclose($fp);

// renaming a file and check stat
echo "*** Testing stat() for files after being renamed ***\n";
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

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation1a.tmp");
?>
--EXPECT--
*** Testing stat() for files after being renamed ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
