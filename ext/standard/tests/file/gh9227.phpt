--TEST--
Bug GH-9227 (Trailing dots and spaces in filenames are ignored)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
var_dump(realpath(__DIR__ . "/gh9227.txt."));
var_dump(file_put_contents(__DIR__ . "/gh9227.txt", "bar"));
var_dump(realpath(__DIR__ . "/gh9227.txt."));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh9227.txt");
?>
--EXPECT--
bool(false)
int(3)
bool(false)
