--TEST--
Test file_get_contents() function : error when passing folder - on Windows
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != "WIN") { print "skip - Windows only"; }
?>
--FILE--
<?php
file_get_contents(__DIR__);
?>
--EXPECTF--
Warning: file_get_contents(%s): Failed to open stream: Permission denied in %s on line %d
