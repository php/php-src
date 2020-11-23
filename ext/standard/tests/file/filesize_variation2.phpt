--TEST--
Test filesize() function: usage variations - size of dir/subdir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip only valid for Linux');
}
--FILE--
<?php
$file_path = __DIR__;
require($file_path."/file.inc");

echo "*** Testing filesize(): usage variations ***\n";

echo "\n*** Testing size of a dir, sub-dir and file with filesize() ***\n";
echo "-- Creating a base dir, and checking its size --\n";
mkdir( $file_path."/filesize_variation2");
var_dump( filesize( $file_path."/filesize_variation2"));
clearstatcache();

echo "-- Creating a file inside base dir, and checking dir & file size --\n";
create_files($file_path."/filesize_variation2", 1, "numeric", 0755, 1, "w", "filesize_variation");
var_dump( filesize( $file_path."/filesize_variation2"));
clearstatcache();
var_dump( filesize( $file_path."/filesize_variation2/filesize_variation1.tmp"));
clearstatcache();
delete_files($file_path."/filesize_variation2", 1, "filesize_variation");

echo "-- Creating an empty sub-dir in base-dir, and checking size of base and sub dir --\n";
mkdir( $file_path."/filesize_variation2/filesize_variation2_sub");
var_dump( filesize( $file_path."/filesize_variation2")); // size of base dir
clearstatcache();
var_dump( filesize( $file_path."/filesize_variation2/filesize_variation2_sub")); // size of subdir
clearstatcache();

echo "-- Creating a file inside sub-dir, and checking size of base, subdir and file created --\n";
// create only the file, as base and subdir is already created
$filename =  $file_path."/filesize_variation2/filesize_variation2_sub/filesize_variation2.tmp";
$file_handle = fopen($filename, "w");
fwrite($file_handle, str_repeat("Hello,World ", 1000) ); // create file of size 12000 bytes
fclose($file_handle);
// size of base dir
var_dump( filesize( $file_path."/filesize_variation2"));
clearstatcache();
// size of subdir
var_dump( filesize( $file_path."/filesize_variation2/filesize_variation2_sub"));
clearstatcache();
// size of file inside subdir
var_dump( filesize( $file_path."/filesize_variation2/filesize_variation2_sub/filesize_variation2.tmp"));
clearstatcache();

echo "*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/filesize_variation2/filesize_variation2_sub/filesize_variation2.tmp");
rmdir($file_path."/filesize_variation2/filesize_variation2_sub");
rmdir($file_path."/filesize_variation2");
?>
--EXPECTF--
*** Testing filesize(): usage variations ***

*** Testing size of a dir, sub-dir and file with filesize() ***
-- Creating a base dir, and checking its size --
int(%d)
-- Creating a file inside base dir, and checking dir & file size --
int(%d)
int(1024)
-- Creating an empty sub-dir in base-dir, and checking size of base and sub dir --
int(%d)
int(%d)
-- Creating a file inside sub-dir, and checking size of base, subdir and file created --
int(%d)
int(%d)
int(12000)
*** Done ***
