--TEST--
Bug #64523: XOR not parsed in INI
--INI--
error_reporting = E_ALL ^ E_NOTICE ^ E_WARNING ^ E_DEPRECATED
--FILE--
<?php
echo ini_get('error_reporting');
?>
--EXPECT--
24565
