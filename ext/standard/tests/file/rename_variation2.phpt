--TEST--
Test rename() function: usage variations-3
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php

$file_path = dirname(__FILE__);

$dest_dir = "$file_path/rename_variation_dir";
// create the $dest_dir
mkdir($dest_dir);

/* Testing rename() on soft and hard links with different permissions */
echo "\n*** Testing rename() on soft links ***\n";
// create the file
$filename = $file_path."/rename_variation1.tmp";
@unlink($filename);
var_dump(touch($filename));

// create the soft links to the file
$linkname = $file_path."/rename_variation_soft_link1.tmp";
var_dump(symlink($filename, $linkname));

//rename the link to a new name in the same dir
$dest_linkname = $file_path."/rename_variation_soft_link2.tmp";
var_dump( rename( $linkname, $dest_linkname) );
//ensure that link was renamed 
clearstatcache();
var_dump( file_exists($linkname) );  // expecting false
var_dump( file_exists($dest_linkname) );  // expecting true

// rename a link across dir
var_dump( rename($dest_linkname, $dest_dir."/rename_variation_soft_link2.tmp"));
//ensure that link got renamed
clearstatcache();
var_dump( file_exists($dest_linkname) );  // expecting false
var_dump( file_exists($dest_dir."/rename_variation_soft_link2.tmp") ); // expecting true

// delete the link file now 
unlink($dest_dir."/rename_variation_soft_link2.tmp");

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/rename_variation1.tmp");
unlink($file_path."/rename_variation_link.tmp");
unlink($file_path."/rename_variation.tmp");
rmdir($file_path."/rename_variation_dir");
?>
--EXPECTF--
*** Testing rename() on soft links ***
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done
