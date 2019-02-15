--TEST--
Test copy() function: usage variations - dir as source
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Run only on Windows");
?>
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy(): Trying to create a copy of an existing dir */

$file_path = dirname(__FILE__);

echo "*** Test copy() function: Trying to create a copy of an existing dir ***\n";
$src_dir = $file_path."/copy_variation12";
mkdir($src_dir);

$dest = $file_path."/copy_copy_variation12";

var_dump( copy($src_dir, $dest) );

var_dump( file_exists($dest) );

var_dump( filesize($src_dir) );

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/copy_copy_variation12");
rmdir(dirname(__FILE__)."/copy_variation12");
?>
--EXPECTF--
*** Test copy() function: Trying to create a copy of an existing dir ***

Warning: copy(): The first argument to copy() function cannot be a directory in %s on line %d
bool(false)
bool(false)
int(0)
*** Done ***
