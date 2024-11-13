--TEST--
Test filesize() function: usage variations - size of files
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
     die('skip Not valid on Windows');
}
?>
--FILE--
<?php
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

echo "Done\n";
?>
--EXPECTF--
*** Testing filesize(): usage variations ***
*** Checking filesize() with different size of files ***
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
Done
