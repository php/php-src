--TEST--
Test lstat() and stat() functions: usage variations - effects of touch() 
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

/* test the effects of touch() on stats of dir/file/link */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file, link and directory */

$dir_name = "$file_path/lstat_stat_variation2/";
mkdir($dir_name);  // temp dir
$file_name = "$file_path/lstat_stat_variation2.tmp";
$fp = fopen($file_name, "w");  // temp file
fclose($fp);
$link_name = "$file_path/lstat_stat_variation_link2.tmp";
symlink($file_name, $link_name);  // temp link


echo "\n*** Checking lstat() and stat() on file, link and directory after touching them ***\n";

// touch a file
echo "-- Testing stat() for file after using touch() on the file --\n";
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


// touch a directory
echo "-- Testing stat() for directory after using touch() on the directory --\n";
$old_stat = stat($dir_name);
// clear the cache
clearstatcache();
sleep(2);
var_dump( touch($dir_name) );
$new_stat = stat($dir_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stat
$affected_members = array(8, 9, 10, 'atime', 'mtime', 'ctime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "<") );
// clear the cache
clearstatcache();

// touch a link
echo "-- Testing lstat() for link after using touch() on the link --\n";
$old_stat = lstat($link_name);
// clear the cache
clearstatcache();
sleep(2);
var_dump( touch($link_name) );
$new_stat = lstat($link_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stat
$affected_members = array(8, 'atime');
var_dump( compare_stats($old_stat, $new_stat, $affected_members, "<") );
// clear the stat
clearstatcache();

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation2.tmp");
rmdir("$file_path/lstat_stat_variation2");
unlink("$file_path/lstat_stat_variation_link2.tmp");
?>
--EXPECTF--
*** Checking lstat() and stat() on file, link and directory after touching them ***
-- Testing stat() for file after using touch() on the file --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing stat() for directory after using touch() on the directory --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing lstat() for link after using touch() on the link --
bool(true)
bool(true)
bool(true)
bool(true)

--- Done ---
