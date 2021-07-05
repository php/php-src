--TEST--
Bug #81223 (flock() only locks first byte of file)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
$filename = __FILE__;
$stream1 = fopen($filename, "r");
var_dump(flock($stream1, LOCK_EX));
$stream2 = fopen($filename, "r");
var_dump(fread($stream2, 5));
fseek($stream2, 1);
var_dump(fread($stream2, 4));
?>
--EXPECTF--
bool(true)

Notice: fread(): read of %d bytes failed with errno=13 Permission denied in %s on line %d
bool(false)

Notice: fread(): read of %d bytes failed with errno=13 Permission denied in %s on line %d
bool(false)
