--TEST--
Test normal operation of PDO::getAvailableDrivers / pdo_drivers
--CREDITS--
Amo Chohan <amo.chohan@gmail.com>
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
--FILE--
<?php
var_dump(is_array(PDO::getAvailableDrivers()));
var_dump(is_array(pdo_drivers()));
--EXPECT--
bool(true)
bool(true)
