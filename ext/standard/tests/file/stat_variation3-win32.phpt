--TEST--
Test stat() functions: usage variations - effects of creating/deleting the dir/file
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php

/*
 *  Prototype: array stat ( string $filename );
 *  Description: Gives information about a file
 */

/* test the effects of creating & deleting of subdir/file  on the stats of dir/file */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file and directory */
mkdir("$file_path/stat_variation3/");  // temp dir

echo "*** Testing stat(): with creating & deleting subdir/file ***\n";

// creating and deleting subdir and files in the dir
echo "-- Testing stat() on dir after subdir and file is created in it --\n";
$dirname = "$file_path/stat_variation3";
$old_stat = stat($dirname);
clearstatcache();
sleep(2);
mkdir("$dirname/stat_variation3_subdir");
$file_handle = fopen("$dirname/stat_variation3a.tmp", "w");
fclose($file_handle);
$new_stat = stat($dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stats
$affected_members = array( 9, 'mtime');
clearstatcache();
sleep(2);
var_dump(compare_stats($old_stat, $new_stat, $affected_members, "<"));
unlink("$dirname/stat_variation3a.tmp");
rmdir("$dirname/stat_variation3_subdir");
clearstatcache();

// comparing stats after the deletion of subdir and file
echo "-- Testing stat() for comparing stats after the deletion of subdir and file --\n";
$new_stat1 = stat($dirname);
// compare self stats
var_dump( compare_self_stat($new_stat1) );
// compare the stats
var_dump(compare_stats($new_stat, $new_stat1, $all_stat_keys, "="));
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir("$file_path/stat_variation3");
?>
--EXPECTF--
*** Testing stat(): with creating & deleting subdir/file ***
-- Testing stat() on dir after subdir and file is created in it --
bool(true)
bool(true)
bool(true)
-- Testing stat() for comparing stats after the deletion of subdir and file --
bool(true)
bool(true)

*** Done ***
