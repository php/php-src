--TEST--
Test lstat() and stat() functions: usage variations - effects of is_dir(), is_file() and is_link()  

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

/* test the effects of is_dir(), is_file() and is_link() on stats of dir/file/link */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file, link and directory */
$dirname = "$file_path/lstat_stat_variation4";
mkdir($dirname);  // temp dir

$filename = "$file_path/lstat_stat_variation4.tmp";
$fp = fopen($filename, "w");  // temp file
fclose($fp);

$linkname = "$file_path/lstat_stat_variation4_link.tmp";
symlink($filename, $linkname); // temp link

echo "\n*** Checking lstat() and stat() on file, link and directory after accessing it
            with is_dir(), is_file() and is_link() functions ***\n";

// is_dir() on a directory
echo "-- Testing on Directory --\n";
$old_stat = stat($dirname);
// clear the cache
clearstatcache();
sleep(2);
var_dump( is_dir($dirname) );
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stat
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );
// clear the stat
clearstatcache();


// is_file() on a file
echo "-- Testing on file --\n";
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

// is_link() on a link
echo "-- Testing on link --\n";
$linkname = "$file_path/lstat_stat_variation4_link.tmp";
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

echo "\n*** Checking stat() on a file with read/write permission ***\n";
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
// clear the stat
clearstatcache();

echo "\n*** Checking lstat() and stat() on hard link ***\n";
$linkname = "$file_path/lstat_stat_variation4_hard.tmp";
var_dump( link($filename, $linkname) );
$file_stat = stat($filename);
$link_stat = lstat($linkname);
// compare self stats
var_dump( compare_self_stat($file_stat) );
var_dump( compare_self_stat($link_stat) );
// compare the stat
var_dump( compare_stats($file_stat, $link_stat, $all_stat_keys) );
// clear the stat
clearstatcache();

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation4_hard.tmp");
unlink("$file_path/lstat_stat_variation4_link.tmp");
unlink("$file_path/lstat_stat_variation4.tmp");
rmdir("$file_path/lstat_stat_variation4");
?>
--EXPECTF--
*** Checking lstat() and stat() on file, link and directory after accessing it
            with is_dir(), is_file() and is_link() functions ***
-- Testing on Directory --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing on file --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing on link --
bool(true)
bool(true)
bool(true)
bool(true)

*** Checking stat() on a file with read/write permission ***
bool(true)
bool(true)
bool(true)

*** Checking lstat() and stat() on hard link ***
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
