--TEST--
GH-19570 (unable to fseek in /dev/zero and /dev/null)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Linux") die("skip only for Linux");
?>
--FILE--
<?php
var_dump(fseek(fopen("/dev/zero", "rb"), 1*1024*1024, SEEK_SET));
var_dump(fseek(fopen("/dev/null", "rb"), 1*1024*1024, SEEK_SET));
?>
--EXPECT--
int(0)
int(0)
