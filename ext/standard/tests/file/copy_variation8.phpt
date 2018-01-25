--TEST--
Test copy() function: usage variations - copying links across dirs
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Invalid for Windows");
?>
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Trying to copy the links across dir paths given in various notations
     and dirs having limited access */

echo "*** Testing copy() function: copying links across different directories ***\n";

$file_path = dirname(__FILE__);

$base_dir = $file_path."/copy_variation8";
mkdir($base_dir);
$sub_dir = $base_dir."/copy_variation8_sub";
mkdir($sub_dir);
$dirname_with_blank = $sub_dir."/copy variation6";
mkdir($dirname_with_blank);

$file = $file_path."/copy_variation8.tmp";
fclose( fopen($file, "w") );

$symlink = $file_path."/copy_variation8_symlink.tmp";
$hardlink = $file_path."/copy_variation8_hardlink.tmp";

symlink($file, $symlink);  //creating symlink
link($file, $hardlink);  //creating hardlink

$dests = array(
  $base_dir."/copy_copy_variation8.tmp",
  $base_dir."/copy_variation8_sub/copy_copy_variation8.tmp",
  "$sub_dir/copy_copy_variation8.tmp",
  "$sub_dir/../copy_copy_variation8.tmp",
  "$sub_dir/../copy_variation8_sub/copy_copy_variation8.tmp",
  "$sub_dir/..///../copy_copy_variation8.tmp",
  "$sub_dir/..///../*",
  "$dirname_with_blank/copy_copy_variation8.tmp"
);

$count = 1;
foreach($dests as $dest) {
  echo "\n-- Iteration $count --\n";
  echo "- With symlink -\n";
  var_dump( copy($symlink, $dest) );
  var_dump( file_exists($dest) );
  var_dump( is_link($dest) ); //expected: bool(false)
  var_dump( is_file($dest) );  //expected: bool(true)
  clearstatcache();
  unlink("$dest");
  echo "- With hardlink -\n";
  var_dump( copy($hardlink, $dest) );
  var_dump( file_exists($dest) );
  var_dump( is_link($dest) );  //expected: bool(flase)
  var_dump( is_file($dest) );  //expected: bool(true)
  clearstatcache();
  unlink("$dest");
  $count++;
}

unlink($symlink);
unlink($hardlink);
unlink($file);
rmdir($dirname_with_blank);
rmdir($sub_dir);
rmdir($base_dir);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing copy() function: copying links across different directories ***

-- Iteration 1 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 2 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 3 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 4 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 5 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 6 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 7 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)

-- Iteration 8 --
- With symlink -
bool(true)
bool(true)
bool(false)
bool(true)
- With hardlink -
bool(true)
bool(true)
bool(false)
bool(true)
*** Done ***
