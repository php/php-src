--TEST--
Test lstat() and stat() functions: usage variations - effects of is_link()
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_OS_FAMILY === 'Windows') {
    include_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

/* test the effects of is_link() on stats of link */

$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp file, link */
$filename = "$file_path/lstat_stat_variation12.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

$linkname = "$file_path/lstat_stat_variation12_link.tmp";
symlink($filename, $linkname); // temp link

// is_link() on a link
echo "*** Testing stat() on a link after using is_link() on it ***\n";
$linkname = "$file_path/lstat_stat_variation12_link.tmp";
$old_stat = lstat($linkname);
// clear the stat
clearstatcache();
sleep(2);
var_dump( is_link($linkname) );
$new_stat = lstat($linkname);
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
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation12_link.tmp");
unlink("$file_path/lstat_stat_variation12.tmp");
?>
--EXPECT--
*** Testing stat() on a link after using is_link() on it ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
