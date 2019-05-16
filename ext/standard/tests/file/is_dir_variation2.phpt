--TEST--
Test is_dir() function: usage variations - links
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Do not run on Windows');
}
--FILE--
<?php
/* Prototype: bool is_dir ( string $dirname );
   Description: Tells whether the dirname is a directory
     Returns TRUE if the dirname exists and is a directory, FALSE  otherwise.
*/

/* Testing is_dir() with dir, soft & hard link to dir,
     and with file, soft & hard link to file */

$file_path = __DIR__;

echo "*** Testing is_dir() with dir and links to dir ***\n";
echo "-- With dir --\n";
$dirname = $file_path."/is_dir_variation2";
mkdir($dirname);
var_dump( is_dir($dirname) );
clearstatcache();

echo "-- With symlink --\n";
symlink($file_path."/is_dir_variation2", $file_path."/is_dir_variation2_symlink");
var_dump( is_dir($file_path."/is_dir_variation2_symlink") );  //is_dir() resolves symlinks
clearstatcache();

echo "-- With hardlink --";
link($file_path."/is_dir_variation2", $file_path."/is_dir_variation2_link"); //Not permitted to create hard-link to a dir
var_dump( is_dir($file_path."/is_dir_variation2_link") );
clearstatcache();

echo "\n*** Testing is_dir() with file and links to a file ***\n";
echo "-- With file --\n";
$filename = $file_path."/is_dir_variation2.tmp";
fclose( fopen($filename, "w") );
var_dump( is_dir($filename) );
clearstatcache();

echo "-- With symlink --\n";
symlink($file_path."/is_dir_variation2.tmp", $file_path."/is_dir_variation2_symlink.tmp");
var_dump( is_dir($file_path."/is_dir_variation2_symlink.tmp") );
clearstatcache();

echo "-- With hardlink --\n";
link($file_path."/is_dir_variation2.tmp", $file_path."/is_dir_variation2_link.tmp");
var_dump( is_dir($file_path."/is_dir_variation2_link.tmp") );
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
if(file_exists($file_path."/is_dir_variation2_symlink")) {
  unlink($file_path."/is_dir_variation2_symlink");
}
if(file_exists($file_path."/is_dir_variation2_symlink")) {
  unlink($file_path."/is_dir_variation2_symlink");
}
if(file_exists($file_path."/is_dir_variation2_symlink.tmp")) {
  unlink($file_path."/is_dir_variation2_symlink.tmp");
}
if(file_exists($file_path."/is_dir_variation2_link.tmp")) {
  unlink($file_path."/is_dir_variation2_link.tmp");
}
if(file_exists($file_path."/is_dir_variation2.tmp")) {
  unlink($file_path."/is_dir_variation2.tmp");
}
if(file_exists($file_path."/is_dir_variation2")) {
  rmdir($file_path."/is_dir_variation2");
}
?>
--EXPECTF--
*** Testing is_dir() with dir and links to dir ***
-- With dir --
bool(true)
-- With symlink --
bool(true)
-- With hardlink --
Warning: link(): %s in %s on line %d
bool(false)

*** Testing is_dir() with file and links to a file ***
-- With file --
bool(false)
-- With symlink --
bool(false)
-- With hardlink --
bool(false)

*** Done ***
