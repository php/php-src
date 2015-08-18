--TEST--
zend multibyte (2)
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
zend.multibyte=On
zend.script_encoding=UTF-8
mbstring.internal_encoding=CP932
--FILE--
<?php
var_dump(bin2hex("テスト"));
?>
--EXPECTF--
string(12) "836583588367"
