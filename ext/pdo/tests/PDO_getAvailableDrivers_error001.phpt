--TEST--
PDO::getAvailableDrivers error when argument supplied
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
?>
--FILE--
<?php
PDO::getAvailableDrivers('fakeArgument');
?>
--EXPECTF--
Warning: PDO::getAvailableDrivers() expects exactly 0 parameters, 1 given in %s on line %d
