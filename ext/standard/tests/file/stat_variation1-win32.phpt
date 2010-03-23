--TEST--
Test stat() functions: usage variations - effects of rename()
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

/* test the effects of rename() on stats of dir/file */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file and directory */
mkdir("$file_path/stat_variation1/");  // temp dir

$file_handle = fopen("$file_path/stat_variation1.tmp", "w");  // temp file
fclose($file_handle);


echo "*** Testing stat(): on file and directory ater renaming them ***\n";

// renaming a file
echo "-- Testing stat() for files after being renamed --\n";
$old_filename = "$file_path/stat_variation1.tmp";
$new_filename = "$file_path/stat_variation1a.tmp";
$old_stat = stat($old_filename);
clearstatcache();
sleep(2);
var_dump( rename($old_filename, $new_filename) );
$new_stat = stat($new_filename);

// compare the self stat 
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats 
var_dump( compare_stats($old_stat, $old_stat, $all_stat_keys) );
// clear the cache
clearstatcache();

// renaming a directory
echo "-- Testing stat() for directory after being renamed --\n";
$old_dirname = "$file_path/stat_variation1";
$new_dirname = "$file_path/stat_variation1a";
$old_stat = stat($old_dirname);
clearstatcache();
sleep(2);
var_dump( rename($old_dirname, $new_dirname) );
$new_stat = stat($new_dirname);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );

// compare the two stats
var_dump( compare_stats($old_stat, $new_stat, $all_stat_keys) );
// clear the cache
clearstatcache();


echo "\n*** Done ***";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/stat_variation1a.tmp");
rmdir("$file_path/stat_variation1a");
?>
--EXPECTF--
*** Testing stat(): on file and directory ater renaming them ***
-- Testing stat() for files after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)
-- Testing stat() for directory after being renamed --
bool(true)
bool(true)
bool(true)
bool(true)

*** Done ***

