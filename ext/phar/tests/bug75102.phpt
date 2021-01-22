--TEST--
Bug #75102 (`PharData` says invalid checksum for valid tar)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
?>
--FILE--
<?php
$phar = new PharData(__DIR__ . '/bug75102.tar');
var_dump(file_get_contents($phar['test.txt']->getPathName()));
?>
--EXPECT--
string(9) "yada yada"
