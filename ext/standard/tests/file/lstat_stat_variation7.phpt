--TEST--
Test lstat() and stat() functions: usage variations - writing data into file
--FILE--
<?php
$file_path = __DIR__;
require "$file_path/file.inc";

/* test the effects on stats with writing data into a  file */

$file_name = "$file_path/lstat_stat_variation7.tmp";
$fp = fopen($file_name, "w");  // temp file
fclose($fp);

// writing to an empty file
echo "*** Testing stat() on file after data is written in it ***\n";
$fh = fopen($file_name,"w");
$old_stat = stat($file_name);
clearstatcache();
$blksize = PHP_OS_FAMILY === 'Windows' ? 4096 : $old_stat['blksize'];
fwrite($fh, str_repeat("Hello World", $blksize));
fclose($fh);
$new_stat = stat($file_name);

// compare self stats
var_dump( compare_self_stat($old_stat) );
var_dump( compare_self_stat($new_stat) );
// compare the stats
$comp_arr = array(7, 'size');
var_dump(compare_stats($old_stat, $new_stat, $comp_arr, "<"));
clearstatcache();

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation7.tmp");
?>
--EXPECT--
*** Testing stat() on file after data is written in it ***
bool(true)
bool(true)
bool(true)

--- Done ---
