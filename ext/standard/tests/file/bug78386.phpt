--TEST--
Bug #78386 (fstat mode has unexpected value on PHP 7.4)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");
?>
--FILE--
<?php
$handle = popen('dir', 'r');
$stat = fstat($handle);
var_dump(decoct($stat['mode']));
?>
--EXPECT--
string(5) "10666"
