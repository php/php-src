--TEST--
Test filesize() function: usage variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip only valid for Linux');
}
--FILE--
<?php
/* 
 Prototype   : int filesize ( string $filename );
 Description : Returns the size of the file in bytes, or FALSE 
   (and generates an error of level E_WARNING) in case of an error.
*/

$file_path = dirname(__FILE__);
require($file_path."/file.inc");

echo "*** Testing filesize(): usage variations ***\n"; 

echo "*** Checking filesize() with different size of files ***\n";
for($size = 1; $size <10000; $size = $size+1000)
{
  create_files($file_path, 1, "numeric", 0755, $size, "w", "filesize_variation");
  var_dump( filesize( $file_path."/filesize_variation1.tmp") );
  clearstatcache();
  delete_files($file_path, 1, "filesize_variation");
}

echo "\n*** Testing size of a dir, sub-dir and file with filesize() ***\n";
echo "-- Creating a base dir, and checking its size --\n";
mkdir( $file_path."/filesize_variation");
var_dump( filesize( $file_path."/filesize_variation"));
clearstatcache();

echo "-- Creating a file inside base dir, and checking dir & file size --\n"; 
create_files($file_path."/filesize_variation", 1, "numeric", 0755, 1, "w", "filesize_variation");
var_dump( filesize( $file_path."/filesize_variation"));
clearstatcache();
var_dump( filesize( $file_path."/filesize_variation/filesize_variation1.tmp"));
clearstatcache();
delete_files($file_path."/filesize_variation", 1, "filesize_variation");

echo "-- Creating an empty sub-dir in base-dir, and checking size of base and sub dir --\n";
mkdir( $file_path."/filesize_variation/filesize_variation_sub");
var_dump( filesize( $file_path."/filesize_variation")); // size of base dir
clearstatcache();
var_dump( filesize( $file_path."/filesize_variation/filesize_variation_sub")); // size of subdir
clearstatcache();

echo "-- Creating a file inside sub-dir, and checking size of base, subdir and file created --\n";
// create only the file, as base and subdir is already created
$filename =  $file_path."/filesize_variation/filesize_variation_sub/filesize_variation.tmp";
$file_handle = fopen($filename, "w");
fwrite($file_handle, str_repeat("Hello,World ", 1000) ); // create file of size 12000 bytes
fclose($file_handle);
// size of base dir
var_dump( filesize( $file_path."/filesize_variation"));
clearstatcache();
// size of subdir
var_dump( filesize( $file_path."/filesize_variation/filesize_variation_sub"));
clearstatcache();
// size of file inside subdir
var_dump( filesize( $file_path."/filesize_variation/filesize_variation_sub/filesize_variation.tmp"));
clearstatcache();

echo "-- Testing filesize() after truncating the file to a new length --\n";
// truncate the file created earlier in subdir, the size of the file is 12000bytes
// truncate the same file, in the loop , each time with the decrement in size by 1200 bytes,
//  until -1200bytes size
for($size = filesize($filename); $size>=-1200; $size-=1200) {
  $file_handle = fopen($filename, "r+");
  var_dump( ftruncate($file_handle, $size) );
  fclose($file_handle);
  var_dump( filesize($filename) );
  clearstatcache();
}

echo "-- Testing filesize() with the data of possible chars --\n";
$filename2 = $file_path."/filesize_variation1.tmp";
$string = "Test 2 test the filesize() fn, with data containing all the types like !@@##$%^&*():<>?|~+!;',.\][{}(special) chars, 12345(numeric) chars, and \n(newline char), \t(tab), \0, \r and so on........\0";
echo "-- opening the file in 'w' mode and get the size --\n";
$file_handle = fopen($filename2, "w");
var_dump( strlen($string) );  //strlen of the string
fwrite($file_handle, $string);
fclose($file_handle);
var_dump( filesize($filename2) );  //size of the file = strlen of string
clearstatcache();

echo "-- opening the file in 'wt' mode and get the size --\n";
$file_handle = fopen($filename2, "wt");
var_dump( strlen($string) );  //strlen of the string = 191 bytes
fwrite($file_handle, $string);
fclose($file_handle);
var_dump( filesize($filename2) );  //size of the file = strlen of string = 191 bytes
clearstatcache();

echo "-- opening the file in 'a' mode, adding data and checking the file --\n";
$file_handle = fopen($filename2, "a");
fwrite($file_handle, "Hello, world");
fclose($file_handle);
var_dump( filesize($filename2) );  //203 bytes
clearstatcache();

echo "-- opening the file in 'at' mode, adding data and checking the file --\n";
$file_handle = fopen($filename2, "at");
fwrite($file_handle, "Hello, world\n");
fclose($file_handle);
var_dump( filesize($filename2) );  //216 bytes
clearstatcache();

echo "-- creating a hole and checking the size --\n";
$file_handle = fopen($filename2, "a");
var_dump( ftruncate($file_handle, 220) );  //creating 4 bytes of hole
fclose($file_handle);
var_dump( filesize($filename2) );  //220 bytes
clearstatcache();

echo "-- writing data after hole and checking the size --\n";
$file_handle = fopen($filename2, "a");
fwrite($file_handle, "Hello\0");  //wrting 6 bytes of data
fclose($file_handle);
var_dump( filesize($filename2) );  //226 bytes
clearstatcache();

echo "-- opening the existing file in write mode --\n";
fclose( fopen($filename2, "w") );
var_dump( filesize($filename2) );  //0 bytes
clearstatcache();

echo "-- with empty file --\n";
$filename3 = dirname(__FILE__)."/filesize_variation_empty.tmp";
fclose( fopen($filename3, "w") );
var_dump( filesize($filename3) );  //0 bytes

echo "*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir($file_path."/filesize_variation");
unlink($file_path."/filesize_variation/filesize_variation_sub/filesize_variation.tmp");
rmdir($file_path."/filesize_variation/filesize_variation_sub");
rmdir($file_path."/filesize_variation");
unlink($file_path."/filesize_variation1.tmp");
unlink($file_path."/filesize_variation_empty.tmp");
?>
--EXPECTF--
*** Testing filesize(): usage variations ***
*** Checking filesize() with different size of files ***
int(1024)
int(1025024)
int(2049024)
int(3073024)
int(4097024)
int(5121024)
int(6145024)
int(7169024)
int(8193024)
int(9217024)

*** Testing size of a dir, sub-dir and file with filesize() ***
-- Creating a base dir, and checking its size --
int(4096)
-- Creating a file inside base dir, and checking dir & file size --
int(4096)
int(1024)
-- Creating an empty sub-dir in base-dir, and checking size of base and sub dir --
int(4096)
int(4096)
-- Creating a file inside sub-dir, and checking size of base, subdir and file created --
int(4096)
int(4096)
int(12000)
-- Testing filesize() after truncating the file to a new length --
bool(true)
int(12000)
bool(true)
int(10800)
bool(true)
int(9600)
bool(true)
int(8400)
bool(true)
int(7200)
bool(true)
int(6000)
bool(true)
int(4800)
bool(true)
int(3600)
bool(true)
int(2400)
bool(true)
int(1200)
bool(true)
int(0)
bool(false)
int(0)
-- Testing filesize() with the data of possible chars --
-- opening the file in 'w' mode and get the size --
int(191)
int(191)
-- opening the file in 'wt' mode and get the size --
int(191)
int(191)
-- opening the file in 'a' mode, adding data and checking the file --
int(203)
-- opening the file in 'at' mode, adding data and checking the file --
int(216)
-- creating a hole and checking the size --
bool(true)
int(220)
-- writing data after hole and checking the size --
int(226)
-- opening the existing file in write mode --
int(0)
-- with empty file --
int(0)
*** Done ***
