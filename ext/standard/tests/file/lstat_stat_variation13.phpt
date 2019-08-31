--TEST--
Test lstat() and stat() functions: usage variations - file opened using w and r mode
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
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

/* use stat on file created using "w" and "r" mode of fopen */

$file_path = __DIR__;
require "$file_path/file.inc";


$filename = "$file_path/lstat_stat_variation13.tmp";

echo "*** Checking stat() on a file opened using read/write mode ***\n";
$file_handle = fopen($filename, "w");  // create file
fclose($file_handle);
$old_stat = stat($filename);
// clear the stat
clearstatcache();
sleep(2);
// opening file again in read mode
$file_handle = fopen($filename, "r");  // read file
fclose($file_handle);
$new_stat = stat($filename);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation13.tmp");
?>
--EXPECT--
*** Checking stat() on a file opened using read/write mode ***
bool(true)
bool(true)
bool(true)

--- Done ---
