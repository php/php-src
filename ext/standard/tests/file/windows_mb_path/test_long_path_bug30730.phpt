--TEST--
Bug #30730 Filename path length limit broken on NTFS volume, using rename
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");

?>
--FILE--
<?php

$dir = dirname(__FILE__) . DIRECTORY_SEPARATOR . "test_bug30730";
$file = $dir . DIRECTORY_SEPARATOR . "test_file";

var_dump(mkdir($dir));

// Create a file in that directory

$fp = fopen($file, 'wb+');
fclose($fp);

// Rename that directory in order that the file full path will be long enough to trigger the bug

$dest_dir =str_pad($dir, 200, '0');
$dest_file = $dest_dir . DIRECTORY_SEPARATOR . "test_file";

var_dump(rename($dir, $dest_dir));

var_dump(file_exists($dest_file));

var_dump(unlink($dest_file));
var_dump(rmdir($dest_dir));

?>
===DONE===
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
