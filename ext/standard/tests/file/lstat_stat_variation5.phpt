--TEST--
Test lstat() and stat() functions: usage variations - effects with changing permissions
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

/* test for effects on stats with changing permissions of dir/file/link */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file, link and directory */
mkdir("$file_path/lstat_stat_variation5/");  // temp dir

$filename = "$file_path/lstat_stat_variation5.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

// temp link
$linkname = "$file_path/lstat_stat_variation5_link.tmp";
symlink($filename, $linkname);

/* Checking lstat() and stat() on file, link and directory after changing permission */

// checking lstat() and stat() on links
echo "\n*** Testing lstat() and stat() on links with miscelleneous file permission and content ***\n";
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
// clear the stat
clearstatcache();

// checking stat() on file
echo "\n*** Testing lstat() and stat() on file with miscelleneous file permission and content ***\n";
$old_stat = stat($filename);
var_dump( chmod($filename, 0777) );
// clear the stat
clearstatcache();
sleep(2);
$new_stat = stat($filename);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array(10, 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "!=") );
// clear the stat
clearstatcache();  // clear statement cache

// checking stat() on directory
echo "\n*** Testing stat() on directory with miscelleneous file permission ***\n";
$dirname = "$file_path/lstat_stat_variation5";
$old_stat = stat($dirname);
var_dump( chmod($dirname, 0777) );
// clear the stat
clearstatcache();
sleep(2);
$new_stat = stat($dirname);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
$affected_members = array(2, 10, 'mode', 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "!=") );

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation5_link.tmp");
unlink("$file_path/lstat_stat_variation5.tmp");
rmdir("$file_path/lstat_stat_variation5");
?>
--EXPECTF--
*** Testing lstat() and stat() on links with miscelleneous file permission and content ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Testing lstat() and stat() on file with miscelleneous file permission and content ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Testing stat() on directory with miscelleneous file permission ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
--UEXPECTF--
*** Testing lstat() and stat() on links with miscelleneous file permission and content ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Testing lstat() and stat() on file with miscelleneous file permission and content ***
bool(true)
bool(true)
bool(true)
bool(true)

*** Testing stat() on directory with miscelleneous file permission ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
