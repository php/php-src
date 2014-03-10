--TEST--
Bug #64523: XOR not parsed in INI
--INI--
error_reporting = E_ALL ^ E_NOTICE ^ E_STRICT ^ E_DEPRECATED
--FILE--
<?php
echo ini_get('error_reporting');
?>
--EXPECTF--
22519
