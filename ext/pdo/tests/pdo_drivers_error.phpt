--TEST--
Test that PDO::getAvailableDrivers / pdo_drivers does not accept any parameters
--CREDITS--
Amo Chohan <amo.chohan@gmail.com>
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
--FILE--
<?php
PDO::getAvailableDrivers('fail');
pdo_drivers('fail');
--EXPECTF--
Warning: PDO::getAvailableDrivers() expects exactly 0 parameters, 1 given in %s on line %d

Warning: pdo_drivers() expects exactly 0 parameters, 1 given in %s on line %d
