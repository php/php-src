--TEST--
Test lstat() and stat() functions: usage variations - effects of touch() on file
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

/* test the effects of touch() on stats of file */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file  */

$file_name = "$file_path/lstat_stat_variation4.tmp";
$fp = fopen($file_name, "w");  // temp file
fclose($fp);

// touch a file check stat, there should be difference in atime
echo "*** Testing stat() for file after using touch() on the file ***\n";
$old_stat = stat($file_name);
// clear the cache
clearstatcache();
sleep(2);
var_dump( touch($file_name) );
$new_stat = stat($file_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stat
$affected_members = array(8, 'atime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "<") );
// clear the cache
clearstatcache();

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation4.tmp");
?>
--EXPECTF--
*** Testing stat() for file after using touch() on the file ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
