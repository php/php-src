--TEST--
Test copy() function: usage variations - destination file access perms
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN')
  die("skip do not run on Windows");
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy(): Trying to copy source file to destination file with and without write permissions */

$file_path = __DIR__;

echo "*** Test copy() function: destination with/without write permissions ***\n";
$src_file_name = $file_path."/copy_variation9.tmp";
$file_handle =  fopen($src_file_name, "w");
fwrite($file_handle, str_repeat("Hello2world...\n", 100));
fclose($file_handle);

$dest_file_name =  $file_path."/copy_copy_variation9.tmp";


echo "\n-- With write permissions --\n";
var_dump( file_exists($src_file_name) );
var_dump( copy($src_file_name, $dest_file_name) );
var_dump( file_exists($dest_file_name) );
var_dump( filesize($dest_file_name) );

echo "\n-- Without write permissions --\n";
chmod($file_path."/copy_copy_variation9.tmp", 0555);  //No write permissions
var_dump( file_exists($src_file_name) );
var_dump( copy($src_file_name, $dest_file_name) );
var_dump( file_exists($dest_file_name) );
var_dump( filesize($dest_file_name) );

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/copy_copy_variation9.tmp");
unlink(__DIR__."/copy_variation9.tmp");
?>
--EXPECTF--
*** Test copy() function: destination with/without write permissions ***

-- With write permissions --
bool(true)
bool(true)
bool(true)
int(1500)

-- Without write permissions --
bool(true)

Warning: %s
bool(false)
bool(true)
int(1500)
*** Done ***
