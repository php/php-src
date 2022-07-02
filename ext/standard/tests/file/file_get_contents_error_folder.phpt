--TEST--
Test file_get_contents() function : error when passing folder
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) === "WIN") { print "skip - not valid for Windows"; }
?>
--FILE--
<?php
file_get_contents(__DIR__);
?>
--EXPECTF--
Notice: file_get_contents(): Read of %d bytes failed with errno=21 Is a directory in %s on line %d
