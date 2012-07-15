--TEST--
PDO::getAvailableDrivers basic
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
?>
--FILE--
<?php
print_r(PDO::getAvailableDrivers());
?>
--EXPECT--
Array
(
    [0] => sqlite
)
