--TEST--
Test copy() function: usage variations - stat after copy 
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy(): checking stat of file before and after after copy operation */

$file_path = dirname(__FILE__);

require($file_path."/file.inc");

echo "*** Test copy() function: stat of file before and after copy ***\n";
$src_file_name = $file_path."/copy_variation18.tmp";
$file_handle =  fopen($src_file_name, "w");
fwrite($file_handle, str_repeat("Hello2world...\n", 100));
fclose($file_handle);

$dest_file_name = $file_path."/copy_copy_variation18.tmp";

clearstatcache();

$stat_before_copy = stat($src_file_name);
clearstatcache();

echo "Copy operation => ";
var_dump( copy($src_file_name, $dest_file_name) ); 

$stat_after_copy = stat($src_file_name);
clearstatcache();

// compare all stat fields except access time
$stat_keys_to_compare = array("dev", "ino", "mode", "nlink", "uid", "gid", 
                       "rdev", "size", "mtime", "ctime",
                       "blksize", "blocks");

echo "Comparing the stats of file before and after copy operation => ";
var_dump( compare_stats($stat_before_copy, $stat_after_copy, $stat_keys_to_compare) );

echo "*** Done ***\n";
?>

--CLEAN--
<?php
unlink(dirname(__FILE__)."/copy_copy_variation18.tmp");
unlink(dirname(__FILE__)."/copy_variation18.tmp");
?>

--EXPECTF--
*** Test copy() function: stat of file before and after copy ***
Copy operation => bool(true)
Comparing the stats of file before and after copy operation => bool(true)
*** Done ***
