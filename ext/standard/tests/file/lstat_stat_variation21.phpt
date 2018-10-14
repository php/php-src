--TEST--
Test lstat() and stat() functions: usage variations - effects of truncate()
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

/* test the effects of truncate() on stats of a file */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file */
$filename = "$file_path/lstat_stat_variation21.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

/* ftruncate the current file and check stat() on the file */

echo "*** Testing stat() on file by truncating it to given size ***\n";
$old_stat = stat($filename);
// clear the cache
clearstatcache();
sleep(2);
// opening file in r/w mode
$file_handle = fopen($filename, "r+");
var_dump( ftruncate($file_handle, 512) );  // truncate it
fclose($file_handle);

$new_stat = stat($filename);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array(7, 9, 10, 'size', 'mtime', 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, '!=') );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation21.tmp");
?>
--EXPECTF--
*** Testing stat() on file by truncating it to given size ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
