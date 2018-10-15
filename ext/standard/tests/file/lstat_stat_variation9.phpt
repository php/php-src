--TEST--
Test lstat() and stat() functions: usage variations - deleting file/subdir
--SKIPIF--
<?php
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

/* test the effects on stats by deleting file/subdir from a dir
*/

echo "*** Testing stat() for comparing stats after the deletion of subdir and file ***\n";

/* first create the dir/subdir and files, record the stat */
@rmdir("$file_path/lstat_stat_variation9/");  // ensure that dir doesn't exists
mkdir("$file_path/lstat_stat_variation9/");  // temp dir

// creating and deleting subdir and files in the dir
$dirname = "$file_path/lstat_stat_variation9";
@rmdir("$dirname/lstat_stat_variation9_subdir"); // ensure that dir doesn't exists
mkdir("$dirname/lstat_stat_variation9_subdir");
$file_handle = fopen("$dirname/lstat_stat_variation9a.tmp", "w");
fclose($file_handle);

$old_stat = stat($dirname);

/* now delete the surdir and file and record the stat */
unlink("$dirname/lstat_stat_variation9a.tmp");
rmdir("$dirname/lstat_stat_variation9_subdir");

// comparing stats after the deletion of subdir and file
$new_stat = stat($dirname);
// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the stats
$affected_members = array(3, 'nlink');
var_dump(compare_stats($old_stat, $new_stat, $affected_members, ">"));

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dirname = "$file_path/lstat_stat_variation9";
rmdir($dirname);
?>
--EXPECTF--
*** Testing stat() for comparing stats after the deletion of subdir and file ***
bool(true)
bool(true)
bool(true)

--- Done ---
