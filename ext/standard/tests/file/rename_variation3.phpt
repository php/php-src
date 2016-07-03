--TEST--
Test rename() function: usage variations-4
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php

$file_path = dirname(__FILE__);

$dest_dir = "$file_path/rename_variation3_dir";
// create the $dest_dir
mkdir($dest_dir);

echo "\n*** Testing rename() on hard links ***\n";
$filename = $file_path."/rename_variation31.tmp";
@unlink($filename);
var_dump(touch($filename));

$linkname = $file_path."/rename_variation3_hard_link1.tmp";
var_dump(link($filename, $linkname));

//rename the link to a new name in the same dir
$dest_linkname = $file_path."/rename_variation3_hard_link2.tmp";
var_dump( rename( $filename, $dest_linkname) );
//ensure that link was renamed
var_dump( file_exists($filename) );  // expecting false
var_dump( file_exists($dest_linkname) );  // expecting true

// rename a hard link across dir
var_dump( rename($dest_linkname, $dest_dir."/rename_variation3_hard_link2.tmp") );
//ensure that link got renamed
var_dump( file_exists($dest_linkname) );  // expecting false
var_dump( file_exists($dest_dir."/rename_variation3_hard_link2.tmp") ); // expecting true

// delete the link file now
unlink($dest_dir."/rename_variation3_hard_link2.tmp");

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/rename_variation3_hard_link1.tmp");
rmdir($file_path."/rename_variation3_dir");
?>
--EXPECTF--
*** Testing rename() on hard links ***
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done
