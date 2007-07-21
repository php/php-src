--TEST--
Test lstat() and stat() functions: usage variations - effects with writing & creating/deleting file/subdir

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

$file_path = dirname(__FILE__);
require "$file_path/file.inc";

/* test the effects on stats with writing data into a  file
   and creating/deleting file/subdir from a dir 
*/

/* create temp file, link and directory */
mkdir("$file_path/lstat_stat_variation3/");  // temp dir

$file_name = "$file_path/lstat_stat_variation3.tmp";
$fp = fopen($file_name, "w");  // temp file
fclose($fp);

symlink("$file_path/lstat_stat_variation3.tmp", "$file_path/lstat_stat_variation3_link.tmp"); // temp link

// writing to an empty file
echo "-- Testing stat() on file after data is written in it --\n";
$fh = fopen($file_name,"w");
$old_stat = stat($file_name);
clearstatcache();
fwrite($fh, "Hello World");
$new_stat = stat($file_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stats
$comp_arr = array(7, 12, 'size', 'blocks');
var_dump(compare_stats($old_stat, $new_stat, $comp_arr, "<"));
clearstatcache();

// creating and deleting subdir and files in the dir
echo "-- Testing stat() on dir after subdir and file is created in it --\n";
$dirname = "$file_path/lstat_stat_variation3";
$old_stat = stat($dirname);
clearstatcache();
sleep(2);
mkdir("$dirname/lstat_stat_variation3_subdir");
$file_handle = fopen("$dirname/lstat_stat_variation3a.tmp", "w");
fclose($file_handle);
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stats
$affected_members = array(3, 9, 10, 'nlink', 'mtime', 'ctime');
clearstatcache();
var_dump(compare_stats($old_stat, $new_stat, $affected_members, "<"));
unlink("$dirname/lstat_stat_variation3a.tmp");
rmdir("$dirname/lstat_stat_variation3_subdir");

// comparing stats after the deletion of subdir and file
echo "-- Testing stat() for comparing stats after the deletion of subdir and file --\n";
$new_stat1 = stat($dirname);
// compare self stats
var_dump( compare_self_stat($new_stat1) );
// compare the stats
$affected_members = array(3, 'nlink');
var_dump(compare_stats($new_stat, $new_stat1, $affected_members, ">"));

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation3.tmp");
rmdir("$file_path/lstat_stat_variation3");
unlink("$file_path/lstat_stat_variation3_link.tmp");
?>
--EXPECTF--
-- Testing stat() on file after data is written in it --
bool(true)
bool(true)
bool(true)
-- Testing stat() on dir after subdir and file is created in it --
bool(true)
bool(true)
bool(true)
-- Testing stat() for comparing stats after the deletion of subdir and file --
bool(true)
bool(true)

--- Done ---
