--TEST--
Test rename() function: usage variations-5
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php

$file_path = dirname(__FILE__);

require dirname(__FILE__).'/file.inc';

/* Renaming a file, link and directory to numeric name */
echo "\n*** Testing rename() by renaming a file, link and directory to numeric name ***\n";
$fp = fopen($file_path."/rename_variation.tmp", "w");
fclose($fp);
// renaming existing file to numeric name
var_dump( rename($file_path."/rename_variation.tmp", $file_path."/12345") );
// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation.tmp" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true
// remove the file
unlink($file_path."/12345");

mkdir($file_path."/rename_variation_dir");

// renaming a directory to numeric name
var_dump( rename($file_path."/rename_variation_dir/", $file_path."/12345") );
// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation_dir" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir($file_path."/12345");
?>
--EXPECTF--
*** Testing rename() by renaming a file, link and directory to numeric name ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done
