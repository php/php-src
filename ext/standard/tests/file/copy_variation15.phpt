--TEST--
Test copy() function: usage variations - destination dir access perms
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN')
  die("skip do not run on Windows");
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__)."/copy_variation15_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
}
unlink($filename);
?>
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy(): Trying to create a copy of file in a dir which doesn't have write permissions */

$file_path = dirname(__FILE__);

echo "*** Test copy() function: Trying to create a copy of file in a dir which doesn't have write permissions ***";
$file = $file_path."/copy_variation15.tmp";
$file_handle =  fopen($file, "w");
fwrite($file_handle, str_repeat(b"Hello, world...", 20));
fclose($file_handle);

$dir = $file_path."/copy_variation15";
mkdir($dir);

$old_perms = fileperms($dir);

chmod($dir, 0555);  //dir without write permissions

$dest = $dir."/copy_copy_variation15.tmp";

var_dump( copy($file, $dir."/copy_copy_variation15.tmp") ); 
var_dump( file_exists($dir."/copy_copy_variation15_dir.tmp") );
var_dump( filesize($file) );  //size of source

chmod($dir, $old_perms);

echo "*** Done ***\n";
?>

--CLEAN--
<?php
unlink(dirname(__FILE__)."/copy_variation15.tmp");
rmdir(dirname(__FILE__)."/copy_variation15");
?>

--EXPECTF--
*** Test copy() function: Trying to create a copy of file in a dir which doesn't have write permissions ***
Warning: copy(%s): %s
bool(false)
bool(false)
int(300)
*** Done ***
