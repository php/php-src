--TEST--
Test filesize() function: usage variations - size of files
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only valid for Windows');
}
--FILE--
<?php
/*
 Prototype   : int filesize ( string $filename );
 Description : Returns the size of the file in bytes, or FALSE
   (and generates an error of level E_WARNING) in case of an error.
*/

$file_path = __DIR__;
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

echo "*** Done ***\n";
?>
--EXPECT--
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
*** Done ***
