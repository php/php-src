--TEST--
Test normal operation of PDO::getAvailableDrivers / pdo_drivers
--CREDITS--
Amo Chohan <amo.chohan@gmail.com>
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_contents();
ob_end_clean();

var_dump(is_array(PDO::getAvailableDrivers()));
var_dump(is_array(pdo_drivers()));
?>
--EXPECT--
bool(true)
bool(true)