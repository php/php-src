--TEST--
file() with various paths
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

$script_directory = __DIR__;
chdir($script_directory);
$test_dirname = basename(__FILE__, ".php") . "testdir";
mkdir($test_dirname);

$filepath = __DIR__ . '/file_variation_5.tmp';
$filename = basename($filepath);
$fd = fopen($filepath, "w+");
fwrite($fd, "Line 1\nLine 2\nLine 3");
fclose($fd);

echo "file() on a path containing .. and .\n";
var_dump(file("./$test_dirname/../$filename"));

echo "\nfile() on a path containing .. with invalid directories\n";
var_dump(file("./$test_dirname/bad_dir/../../$filename"));

echo "\nfile() on a linked file\n";
$linkname = "somelink";
var_dump(symlink($filepath, $linkname));
var_dump(file($linkname));
var_dump(unlink($linkname));

echo "\nfile() on a relative path from a different working directory\n";
chdir($test_dirname);
var_dump(file("../$filename"));
chdir($script_directory);

?>
--CLEAN--
<?php
$test_dirname = __DIR__ . '/' . basename(__FILE__, ".clean.php") . "testdir";
$filepath = __DIR__ . '/file_variation_5.tmp';
@unlink($filepath); // Should be delete via the symlink deletion
rmdir($test_dirname);
?>
--EXPECT--
file() on a path containing .. and .
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

file() on a path containing .. with invalid directories
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

file() on a linked file
bool(true)
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
bool(true)

file() on a relative path from a different working directory
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
