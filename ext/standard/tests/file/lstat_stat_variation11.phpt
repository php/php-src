--TEST--
Test lstat() and stat() functions: usage variations - effect of is_file()
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

$file_path = dirname(__FILE__);
require "$file_path/file.inc";

/* test the effects of is_file() on stats of a file */

/* create temp file */
$filename = "$file_path/lstat_stat_variation11.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

// is_file() on a file
echo "*** Testing stat() on a file after using is_file() on it ***\n";
$old_stat = stat($filename);
// clear the stat
clearstatcache();
sleep(2);
var_dump( is_file($filename) );
$new_stat = stat($filename);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );
// clear the stat
clearstatcache();

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation11.tmp");
?>
--EXPECTF--
*** Testing stat() on a file after using is_file() on it ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
