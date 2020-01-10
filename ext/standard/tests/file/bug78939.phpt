--TEST--
Bug #78939 (Windows relative path with drive letter)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) !== 'WIN') die('skip test is for Windows only');
?>
--FILE--
<?php
list($drive, $dirA, $dirB) = explode('\\', __FILE__, 4);
var_dump(file_exists($drive . '\\'));
var_dump(file_exists($drive . '\\' . $dirA));
var_dump(file_exists($drive . '\\' . $dirA . '\\' . $dirB));
var_dump(is_dir($drive . '\\' . $dirA . '\\' . $dirB));
echo "\n";

var_dump(chdir($drive . '\\'));
var_dump(file_exists($drive . $dirA));
var_dump(is_dir($drive . $dirA));
var_dump(file_exists($drive . $dirB));
var_dump(is_dir($drive . $dirB));
var_dump(file_exists($drive . $dirA . '/' . $dirB));
var_dump(is_dir($drive . $dirA . '/' . $dirB));
echo "\n";

var_dump(chdir($drive . '\\' . $dirA));
var_dump(file_exists($drive . $dirA));
var_dump(is_dir($drive . $dirA));
var_dump(file_exists($drive . $dirB));
var_dump(is_dir($drive . $dirB));
var_dump(file_exists($drive . $dirA . '/' . $dirB));
var_dump(is_dir($drive . $dirA . '/' . $dirB));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)

bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)

bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
