--TEST--
Test stat() functions: usage variations - effects of writing to file
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php

/* test the effects of writing to a file on the stats of the file */


$file_path = __DIR__;
require "$file_path/file.inc";


$filename = "$file_path/stat_variation2.tmp";
$file_handle = fopen($filename, "w");  // temp file
fclose($file_handle);


echo "*** Testing stat(): writing to a file ***\n";

// writing to an empty file
echo "-- Testing stat() on file after data is written in it --\n";
$old_stat = stat($filename);
clearstatcache();
sleep(1);
$file_handle = fopen($filename, "w");  // temp file
fwrite($file_handle, "Hello World");
fclose($file_handle);
$new_stat = stat($filename);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stats
$comp_arr = array(7, 'size');
var_dump(compare_stats($old_stat, $new_stat, $comp_arr, "<"));
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/stat_variation2.tmp");
?>
--EXPECT--
*** Testing stat(): writing to a file ***
-- Testing stat() on file after data is written in it --
bool(true)
bool(true)
bool(true)

*** Done ***
