--TEST--
Test lstat() and stat() functions: usage variations - effects changing permissions of link
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
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

/* test the effects on stats by changing permissions of link */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


$filename = "$file_path/lstat_stat_variation15.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

// temp link
$linkname = "$file_path/lstat_stat_variation15_link.tmp";
symlink($filename, $linkname);

// checking lstat() and stat() on links
echo "*** Testing lstat() on a link after changing its access permission ***\n";
clearstatcache();
$old_stat = lstat($linkname);
var_dump( chmod($linkname, 0777) );
// clear the stat
clearstatcache();
sleep(2);
$new_stat = lstat($linkname);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys, "=") );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation15_link.tmp");
unlink("$file_path/lstat_stat_variation15.tmp");
?>
--EXPECT--
*** Testing lstat() on a link after changing its access permission ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
